GR_SWIG_BLOCK_MAGIC(pager,flex_parse);

pager_flex_parse_sptr pager_make_flex_parse(gr_msg_queue_sptr queue, float freq);

class pager_flex_parse : public gr_block
{
private:
    pager_flex_parse(gr_msg_queue_sptr queue, float freq);

public:
};
