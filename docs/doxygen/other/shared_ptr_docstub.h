namespace std {
/*!
 * \brief shared_ptr documentation stub
 *
 * \warning
 * This isn't the real shared_ptr template.  It's just enough to get doxygen
 * to draw pretty collaboration diagrams.
 *
 * An enhanced relative of scoped_ptr with reference counted copy semantics.
 * The object pointed to is deleted when the last shared_ptr pointing to it
 * is destroyed or reset.
 */

template <class T>
class shared_ptr
{
public:
    T* px; // contained pointer

}; // shared_ptr


} // namespace std
