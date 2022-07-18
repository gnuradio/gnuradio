#include <gnuradio/buffer_management.h>

namespace gr {

void buffer_manager::initialize_buffers(flat_graph_sptr fg,
                                        std::shared_ptr<buffer_properties> buf_props,
                                        neighbor_interface_sptr sched_intf)
{
    // not all edges may be used
    for (auto e : fg->stream_edges()) {
        // every edge needs a buffer
        auto num_items = get_buffer_num_items(e, fg);

        if (e->src().port()) {
            // If buffer has not yet been created, e.g. 1:N block connection
            if (!e->src().port()->get_buffer()) {

                // If src block is in this domain
                if (std::find(fg->nodes().begin(), fg->nodes().end(), e->src().node()) !=
                    fg->nodes().end()) {

                    buffer_uptr buf;
                    if (e->has_custom_buffer()) {
                        buf = e->buffer_factory()(
                            num_items, e->itemsize(), e->buf_properties());
                    }
                    else {
                        buf = buf_props->factory()(num_items, e->itemsize(), buf_props);
                    }

                    d_debug_logger->debug(
                        "Edge: {}, Buf: {}, {} bytes, {} items of size {}",
                        e->identifier(),
                        buf->type(),
                        buf->buf_size(),
                        buf->num_items(),
                        buf->item_size());

                    e->src().port()->set_buffer(std::move(buf));
                }
            }
            else {
                auto buf = e->src().port()->get_buffer();
                d_debug_logger->debug(
                    "Edge: {}, Buf(copy): {}, {} bytes, {} items of size {}",
                    e->identifier(),
                    buf->type(),
                    buf->buf_size(),
                    buf->num_items(),
                    buf->item_size());
            }
        }
    }

    // Assuming all the buffers that the readers will be attaching to have been created at
    // this point.  Will need to handle crossings separately if doing something complex
    for (auto& b : fg->calc_used_blocks()) {
        port_vector_t input_ports = b->input_stream_ports();
        port_vector_t output_ports = b->output_stream_ports();

        for (auto p : input_ports) {
            edge_vector_t ed = fg->find_edge(p);
            if (ed.empty()) {
                if (p->optional()) {
                    continue;
                }
                else {
                    throw std::runtime_error("Edge associated with input port not found");
                }
            }

            // TODO: more robust way of ensuring readers don't get double-added
            // If dst block is in this domain, then add the reader to the source port
            if (std::find(fg->nodes().begin(), fg->nodes().end(), ed[0]->dst().node()) !=
                fg->nodes().end()) {

                if (ed[0]->buf_properties() &&
                    ed[0]->buf_properties()->reader_factory()) {
                    d_debug_logger->debug(
                        "Creating Buffer Reader for Edge: {}, Independently",
                        ed[0]->identifier());
                    auto br = ed[0]->buf_properties()->reader_factory()(
                        ed[0]->dst().port()->itemsize(), ed[0]->buf_properties());
                    p->set_buffer_reader(std::move(br));
                    p->get_buffer_reader()->set_parent_intf(sched_intf);
                }
                else {

                    d_debug_logger->debug(
                        "Adding Buffer Reader for Edge: {}, to buffer on Block {}",
                        ed[0]->identifier(),
                        ed[0]->src().identifier());
                    auto br = ed[0]->src().port()->get_buffer()->add_reader(
                        ed[0]->buf_properties(), ed[0]->dst().port()->itemsize());
                    p->set_buffer_reader(std::move(br));
                }
            }
        }
    }
}

int buffer_manager::get_buffer_num_items(edge_sptr e, flat_graph_sptr fg)
{
    size_t item_size = e->itemsize();

    // *2 because we're now only filling them 1/2 way in order to
    // increase the available parallelism when using the TPB scheduler.
    // (We're double buffering, where we used to single buffer)

    size_t buf_size = s_fixed_buf_size;
    if (e->has_custom_buffer()) {

        auto req_buf_size = e->buf_properties()->buffer_size();

        if (req_buf_size > 0) {
            buf_size = req_buf_size;
        }
        else {
            auto max_buf_size = e->buf_properties()->max_buffer_size();
            auto min_buf_size = e->buf_properties()->min_buffer_size();
            if (max_buf_size > 0) {
                buf_size = std::min(buf_size, max_buf_size);
            }
            if (min_buf_size > 0) {
                buf_size = std::max(buf_size, min_buf_size);
            }
        }
    }

    size_t nitems = item_size == 0 ? 0 : (buf_size * 2) / item_size;

    auto grblock = std::dynamic_pointer_cast<block>(e->src().node());
    if (grblock == nullptr) // might be a domain adapter, not a block
    {
        grblock = std::dynamic_pointer_cast<block>(e->dst().node());
    }

    if (grblock->output_multiple_set()) {
        nitems = std::max(nitems, static_cast<size_t>(2 * (grblock->output_multiple())));
    }

    // If any downstream blocks are decimators and/or have a large output_multiple,
    // ensure we have a buffer at least twice their decimation
    // factor*output_multiple

    auto blocks = fg->calc_downstream_blocks(grblock, e->src().port());

    for (auto& p : blocks) {
        // block_sptr dgrblock = cast_to_block_sptr(*p);
        // if (!dgrblock)
        //     throw std::runtime_error("allocate_buffer found non-gr::block");

        // double decimation = (1.0 / dgrblock->relative_rate());
        double decimation = (1.0 / p->relative_rate());
        int multiple = p->output_multiple();
        nitems = std::max(nitems, static_cast<size_t>(2 * (decimation * multiple)));
        // std::max(nitems, static_cast<int>(2 * (decimation * multiple)));
    }

    return nitems;
}

} // namespace gr
