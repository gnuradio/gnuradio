GR_SWIG_BLOCK_MAGIC(pager,slicer_fb);

pager_slicer_fb_sptr pager_make_slicer_fb(float alpha);

class pager_slicer_fb : public gr_sync_block
{
private:
    pager_slicer_fb(float alpha);

public:
    float dc_offset() const { return d_avg; }
};
