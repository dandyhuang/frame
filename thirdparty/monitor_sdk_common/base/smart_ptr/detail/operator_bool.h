
#ifndef COMMON_BASE_SMART_PTR_OPERATOR_BOOL_H
#define COMMON_BASE_SMART_PTR_OPERATOR_BOOL_H
    
typedef T * (this_type::*unspecified_bool_type)() const;
    
operator unspecified_bool_type() const // never throws
{
    return px == 0? 0: &this_type::get;
}
    
// operator! is redundant, but some compilers need it
bool operator! () const // never throws
{
    return px == 0;
}

#endif // COMMON_BASE_SMART_PTR_OPERATOR_BOOL_H

