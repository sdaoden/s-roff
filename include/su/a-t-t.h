/*@ C++ auto_type_toolbox<T>.  For the lazy sort.
 *@ If included with su_A_T_T_DECL_ONLY defined (and not fully included before)
 *@ then no external headers will be included etc.: this is nice for providing
 *@ auto_type_toolbox specializations.
 *
 * Copyright (c) 2003 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef su_A_T_T_H
#ifndef su_A_T_T_DECL_ONLY
# define su_A_T_T_H
#endif

#include <su/code.h>
#if !C_LANG /* Not indented for this */

#ifndef su_A_T_T_DECL_ONLY
# include <su/mem.h>
#endif

#define su_HEADER
#include <su/code-in.h>
NSPC_BEGIN(su)

template<class T> class auto_type_toolbox;

#ifndef su_A_T_T_DECL_OK
// Note: if T is a pointer type, the a-t-t will still create heap clones,
// so T* and T are in fact treated alike
template<class T>
class auto_type_toolbox{
public:
   typedef NSPC(su)type_traits<T> type_traits;

   // Accessing this field should be avoided because there may be
   // specializations which do not offer it -- get_instance() is inline
   static type_toolbox<T> const instance;

   static type_toolbox<T> const *get_instance(void) {return &instance;}

private:
   static typename type_traits::ptr s_clone(typename type_traits::const_ptr t);
   static void s_delete(typename type_traits::ptr self);
   static typename type_traits::ptr s_assign(typename type_traits::ptr self,
         typename type_traits::const_ptr t);
   static sz s_compare(typename type_traits::const_ptr self,
         typename type_traits::const_ptr t);
   static uz s_hash(typename type_traits::const_ptr self);
};
# define su_A_T_T_DECL_OK
#endif /* su_A_T_T_DECL_OK */

#ifdef su_A_T_T_DECL_ONLY
# undef su_A_T_T_DECL_ONLY
#else
template<class T>
PRI STA typename type_traits::ptr
auto_type_toolbox<T>::s_clone(typename type_traits::const_ptr t){
   ASSERT_RET(t != NIL, NIL);
   return su_NEW(typename type_traits::type)(*t);
}

template<class T>
PRI STA void
auto_type_toolbox<T>::s_delete(typename type_traits::ptr self){
   ASSERT_RET(self != NIL, );
   su_DEL(self);
}

template<class T>
PRI STA typename type_traits::ptr
auto_type_toolbox<T>::s_assign(typename type_traits::ptr self,
      typename type_traits::const_ptr t){
   ASSERT_RET(self != NIL, NIL);
   ASSER_RET(t != NIL, self);
   if(self != t)
      *self = *t;
   return self;
}

template<class T>
PRI STA sz
auto_type_toolbox<T>::s_compare(typename type_traits::const_ptr self,
      typename type_traits::const_ptr t){
   ASSERT_RET(self != NIL, (t != NIL) ? -1 : 0);
   ASSERT_RET(t != NIL, 1);
   return self->compare(*t);
}

template<class T>
PRI STA uz
auto_type_toolbox<T>::s_hash(typename type_traits::const_ptr self){
   ASSERT_RET(self != NIL, 0);
   return self->hash();
}

template<class T>
PUB STA type_toolbox<T> const auto_type_toolbox<T>::instance =
      su_TYPE_TOOLBOX_I9R(&s_clone, &s_delete, &s_assign, &s_compare, &s_hash);
#endif // !su_A_T_T_DECL_ONLY

NSPC_END(su)
#include <su/code-ou.h>
#endif /* !C_LANG */
#endif /* su_A_T_T_H */
/* s-it-mode */
