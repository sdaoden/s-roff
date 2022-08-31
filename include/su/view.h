/*@ Generic C++ View (-of-a-collection, for iterating plus purposes) templates.
 *@ (Merely of interest when creating a new C++ collection type.)
 *
 * Copyright (c) 2001 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
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
#ifndef su_VIEW_H
#define su_VIEW_H

#include <su/code.h>
#if !C_LANG /* Not indented for this */

#define su_HEADER
#include <su/code-in.h>
NSPC_BEGIN(su)

template<class BASECOLLT, class KEYT, class T> class view_traits;
template<class VIEWTRAITS, class GBASEVIEWT> class view__base;
template<class VIEWTRAITS, class GBASEVIEWT> class view_unidir;
template<class VIEWTRAITS, class GBASEVIEWT> class const_view_unidir;
template<class VIEWTRAITS, class GBASEVIEWT> class view_bidir;
template<class VIEWTRAITS, class GBASEVIEWT> class const_view_bidir;
template<class VIEWTRAITS, class GBASEVIEWT> class view_random;
template<class VIEWTRAITS, class GBASEVIEWT> class const_view_random;
template<class VIEWTRAITS, class GBASEVIEWT> class view_assoc_unidir;
template<class VIEWTRAITS, class GBASEVIEWT> class const_view_assoc_unidir;
template<class VIEWTRAITS, class GBASEVIEWT> class view_assoc_bidir;
template<class VIEWTRAITS, class GBASEVIEWT> class const_view_assoc_bidir;

/*!@
 * \group{DESCRIPTION}
 *
 * \group2{Superclass requirements}
 *
 * The minimum required interface that the typesafe C++ templates require,
 * and its expected behaviour.
 * In general there is a basic interface and several type-specific additions
 * which stack upon each other.
 *
 * \group3{Base}
 *
 * \list
 * \li{Copy constructor}
 * \li{Assignment operator (C: \c{_assign()})}
 *
 * \li{\fun{self_class &tie(super_collection &tc);}}
 * \li{\fun{boole is_tied(void) const;}}
 * \li{\fun{boole is_same_tie(self_class const &t) const;}}
 *    Create a tie in between this view and a collection instance.
 *
 * \li{\fun{boole is_valid(void) const;}}
 * \li{\fun{self_class &invalidate(void);}}
 *    A valid view points to an accessible slot of the collection instance.
 *
 * \li{\fun{void *data(void);}}
 * \li{\fun{void const *data(void) const;}}
 *    \NIL is returned if \c{is_valid()} assertion triggers.
 *
 * \li{\fun{self_class &begin(void);}}
 *    Move the \c{is_tied()} view to the first iteratable position, if any.
 *
 * \li{\fun{boole has_next(void) const;}}
 * \li{\fun{self_class &next(void);}}
 *    Step the \c{is_valid()} view to the next position, if any.
 *
 * \li{\fun{self_class &remove(void);}}
 *    Remove the current \c{is_valid()} entry, and move to the next position,
 *    if there is any.
 *
 * \li{\fun{sz compare(self_class const &t) const;}}
 *    This need not compare all possible cases, only those which make sense.
 *    Equality and inequality must be provided.
 *    Neither \c{is_tied()} nor \c{is_valid()} are preconditions for
 *    comparison; a non-\c{tie()}d view shall compare less-than an
 *    \c{is_tied()} one, ditto \c{is_valid()}.
 * \end
 *
 * \group3{Additions for non-associative views}
 *
 * \list
 * \li{\fun{s32 insert(void const *dat);}}
 *    Insert new element after current position if that \c{is_valid()},
 *    otherwise creates a new \c{begin()}.
 *    Returns \#su_ERR_NONE and is positioned at the inserted element upon
 *    success, and \#su_ERR_INVAL if the \c{is_tied()} assertion fails.
 * \li{\fun{s32 insert_range(self_class &startpos, self_class &endpos);}}
 *    Insert new element(s) after current position if that \c{is_valid()},
 *    otherwise creates a new \c{begin()} first.
 *    \c{startpos} must be \c{is_valid()} and must not be \c{is_same_tie()}
 *    as \THIS \em if it is, it is asserted that ranges do not overlap.
 *    All positions accessible by iterating \c{startpos} will be inserted.
 *    If \c{endpos} is \c{is_valid()} \c{startpos.is_same_tie(endpos)} must
 *    hold and the iteration does not include \c{endpos}.
 *    Returns \#su_ERR_NONE and is positioned at the last inserted element
 *    upon success, and \#su_ERR_INVAL if the \c{is_tied()} or any of the
 *    argument assertions fail, among others.
 *
 * \li{\fun{self_class &remove_range(self_class &endpos);}}
 *    Remove all elements starting at the current \c{is_valid()} entry unless
 *    \THIS becomes invalid, or a given \c{is_valid()} \c{endpos}ition is
 *    reached, which is not removed.
 *    If \c{endpos} \c{is_valid()} then it must be \c{tie()}d to the same
 *    parent as this view; it may also become updated in this case in order to
 *    stay at the same effective position after the operation completed:
 *    for example, removing a range from an array requires \c{endpos} to be
 *    moved further to "the front".
 * \end
 *
 * \group3{Additions for associative views}
 *
 * \list
 * \li{\fun{void const *key(void) const;}}
 *    \NIL is returned if the \c{is_valid()} assertion triggers.
 * \end
 *
 * \group3{Additions for non-associative unidirectional views}
 *
 * \list
 * \li{\fun{self_class &go_to(uz off);}}
 *    Move the \c{is_tied()} view to the absolute position \c{off}.
 *
 * \li{\fun{boole find(void const *dat, boole byptr);}}
 *    Search for \c{dat} in the \c{is_tied()} view, either starting at the
 *    current position if \c{is_valid()}, at \c{begin()} otherwise.
 *    \c{byptr} states whether data shall be found by simple pointer
 *    comparison; if not, the \#toolbox of the \c{tied()}d parent should be
 *    asserted (if used by the collection in question).
 * \end
 *
 * \group3{Additions for associative unidirectional views}
 *
 * \list
 * \li{\fun{boole find(void const *key);}}
 *    Search for \c{key} in the \c{is_tied()} view.
 * \end
 *
 * \group3{Additions for bidirectional views}
 *
 * \list
 * \li{\fun{self_class &end(void);}}
 *    Move the \c{is_tied()} view to the last position, if any.
 *
 * \li{\fun{boole has_last(void) const;}}
 * \li{\fun{self_class &last(void);}}
 *    Move the \c{is_valid()} view to the position before the current one,
 *    if any.
 * \end
 *
 * \group3{Additions for non-associative bidirectional views}
 *
 * \list
 * \li{\fun{boole rfind(void const *dat, boole byptr);}}
 *    Search for \c{dat} in the \c{is_tied()} view, either starting at the
 *    current position if \c{is_valid()}, at \c{end()} otherwise.
 *    \c{byptr} states whether data shall be found by simple pointer
 *    comparison; if not, the \#toolbox of the \c{tied()}d parent should be
 *    asserted.
 * \end
 *
 * \group3{Additions for non-associative random-access views}
 *
 * This type extends the requirement of the \c{compare()} function in the base
 * set, and requires the additional results less-than, less-than-or-equal,
 * greater-than and greater-than-or-equal.
 *
 * \list
 * \li{\fun{self_class &go_around(sz reloff);}}
 *    Move the \c{is_valid()} view relative by \c{reloff} positions.
 * \end
 */

enum view_category{
   view_category_non_assoc,
   view_category_assoc
};

enum view_type{
   view_type_unidir,
   view_type_bidir,
   view_type_random,
   view_type_assoc_unidir,
   view_type_assoc_bidir
};

template<class BASECOLLT, class KEYT, class T>
class view_traits{
public:
   typedef BASECOLLT base_coll_type;

   // Identical to normal traits except for view_category_assoc views
   typedef NSPC(su)type_traits<KEYT> key_type_traits;
   typedef NSPC(su)type_traits<T> type_traits;
};

template<class VIEWTRAITS, class GBASEVIEWT>
class view__base{
protected:
   GBASEVIEWT m_view;

   view__base(void) : m_view() {}
   view__base(view__base const &t) : m_view(t.m_view) {}

public:
   ~view__base(void) {}

protected:
   view__base &assign(view__base const &t){
      m_view = t.m_view;
      return *this;
   }
   view__base &operator=(view__base const &t) {return assign(t);}

public:
   boole is_tied(void) const {return m_view.is_tied();}

   boole is_same_tie(view__base const &t) const{
      return m_view.is_same_tie(t.m_view);
   }

   boole is_valid(void) const {return m_view.is_valid();}
   operator boole(void) const {return is_valid();}

protected:
   view__base &invalidate(void){
      (void)m_view.invalidate();
      return *this;
   }

   sz compare(view__base const &t) const {return m_view.compare(t.m_view);}

public:
   boole is_equal(view__base const &t) const {return (compare(t) == 0);}
   boole operator==(view__base const &t) const {return (compare(t) == 0);}
   boole operator!=(view__base const &t) const {return (compare(t) != 0);}
};

// Because of the various sorts of views we define helper macros.
// Unfortunately GCC (up to and including 3.4.2) cannot access
// base& _x   ...   _x.m_view
// ("is protected within this context"), but can only access
// base& _x   ...   MYSELF& y = _x   ...   y.m_view
// (and only so if we put a "using" directive, see su__VIEW_IMPL_START__BASE).
// To be (hopefully..) absolutely safe use a C-style cast
#define su__VIEW_DOWNCAST(X) ((su__VIEW_NAME&)X)

#define su__VIEW_IMPL_START /*{*/\
template<class VIEWTRAITS, class GBASEVIEWT>\
class su__VIEW_NAME : public view__base<VIEWTRAITS,GBASEVIEWT>{\
   typedef su__VIEW_NAME<VIEWTRAITS,GBASEVIEWT> myself;\
   typedef view__base<VIEWTRAITS,GBASEVIEWT> base;\
   \
   /* XXX All these typedefs could be moved to class view__base!? */\
   typedef typename VIEWTRAITS::base_coll_type base_coll_type;\
   typedef typename VIEWTRAITS::key_type_traits key_type_traits;\
   typedef typename VIEWTRAITS::type_traits type_traits;\
   \
   /* (Simply add _key_ - for non-associative this is eq const_ptr) */\
   typedef typename key_type_traits::const_ptr const_key_ptr;\
   typedef typename type_traits::type type;\
   typedef typename type_traits::ptr ptr;\
   typedef typename type_traits::const_ptr const_ptr;\
   \
protected:\
   /* (GCC (up to and incl. 3.4.2) does not find it otherwise) */\
   using base::m_view;\
   \
public:\
   static view_category const category = su__VIEW_CATEGORY;\
   static view_type const type = su__VIEW_TYPE;\
   \
   su__VIEW_NAME(void) : base() {}\
   template<class TCOLL> explicit su__VIEW_NAME(TCOLL &tc) : base(){\
      (void)m_view.tie(S(base_coll_type&,tc)).begin();\
   }\
   /* (Need to offer all forms to allow additional TCOLL template(s)..) */\
   su__VIEW_NAME(su__VIEW_NAME &t) : base(t) {}\
   su__VIEW_NAME(su__VIEW_NAME const &t) : base(t) {}\
   ~su__VIEW_NAME(void) {}\
   \
   su__VIEW_NAME &assign(su__VIEW_NAME const &t){\
      return S(myself&,base::assign(t));\
   }\
   su__VIEW_NAME &operator=(su__VIEW_NAME const &t) {return assign(t);}\
   \
   using base::is_tied;\
   using base::is_same_tie;\
   template<class TCOLL> su__VIEW_NAME &tie(TCOLL &tc){\
      (void)m_view.tie(S(base_coll_type&,tc));\
      return *this;\
   }\
   \
   using base::is_valid;\
   using base::operator boole;\
   \
   su__VIEW_NAME &invalidate(void) {return S(myself&,base::invalidate());}
   \
   const_ptr data(void) const{\
      ASSERT_RET(is_valid(), NIL);\
      return type_traits::to_const_ptr(m_view.data());\
   }\
   const_ptr operator*(void) const {return data();}\
   const_ptr operator->(void) const {return data();}\
   \
   su__VIEW_NAME &begin(void){\
      ASSERT_RET(is_tied(), *this);\
      (void)m_view.begin();\
      return *this;\
   }\
   template<class TCOLL> su__VIEW_NAME &begin(TCOLL &tc){\
      (void)m_view.tie(S(base_coll_type&,tc)).begin();\
      return *this;\
   }\
   \
   boole has_next(void) const{\
      ASSERT_RET(is_valid(), FAL0);\
      return m_view.has_next();\
   }\
   su__VIEW_NAME &next(void){\
      ASSERT_RET(is_valid(), *this);\
      (void)m_view.next();\
      return *this;\
   }\
   su__VIEW_NAME &operator++(void) {return next();}
   \
   using base::is_equal;\
   using base::operator==;\
   using base::operator!=;\
/*}}*/

#define su__VIEW_IMPL_NONCONST /*{*/\
   ptr data(void){\
      ASSERT_RET(is_valid(), NIL);\
      return type_traits::to_ptr(m_view.data());\
   }\
   ptr operator*(void) {return data();}\
   ptr operator->(void) {return data();}\
   \
   su__VIEW_NAME &remove(void){\
      ASSERT_RET(is_valid(), *this);\
      (void)m_view.remove();\
      return *this;\
   }\
/*}*/

#define su__VIEW_IMPL_CONST /*{*/\
   /* (We need to cast away the 'const', but it is preserved logically..) */\
   template<class TCOLL> explicit su__VIEW_NAME(TCOLL const &tc){\
      (void)m_view.tie(S(base_coll_type&,C(TCOLL&,tc))).begin();\
   }\
   /* (Need to offer all copy-forms to allow TCOLL template..) */\
   explicit su__VIEW_NAME(su__NONCONST_VIEW_NAME<VIEWTRAITS,GBASEVIEWT> &t)\
         : base(t){\
   }\
   explicit su__VIEW_NAME(\
         su__NONCONST_VIEW_NAME<VIEWTRAITS,GBASEVIEWT> const &t) : base(t) {}\
   \
   su__VIEW_NAME &assign(\
         su__NONCONST_VIEW_NAME<VIEWTRAITS,GBASEVIEWT> const &t){\
      return S(myself&,base::assign(t));\
   }\
   su__VIEW_NAME &operator=(\
         su__NONCONST_VIEW_NAME<VIEWTRAITS,GBASEVIEWT> const &t){\
      return assign(t);\
   }\
   \
   /* (We need to cast away the 'const', but it is preserved logically..) */\
   template<class TCOLL> su__VIEW_NAME &tie(TCOLL const &tc){\
      (void)m_view.tie(S(base_coll_type&,C(TCOLL&,tc)));\
      return *this;\
   }\
/*}*/

#define su__VIEW_IMPL_NONASSOC

#define su__VIEW_IMPL_NONASSOC_NONCONST /*{*/\
   /* su_ERR_NONE or error */\
   s32 insert(const_ptr dat){\
      ASSERT_RET(is_tied(), su_ERR_INVAL);\
      return m_view.insert(type_traits::to_const_vptr(dat));\
   }\
   s32 insert(base &startpos, base const &endpos){\
      ASSERT_RET(is_tied(), su_ERR_INVAL);\
      ASSERT_RET(startpos.is_valid(), su_ERR_INVAL);\
      ASSERT_RET(!endpos.is_valid() || startpos.is_same_tie(endpos),\
         su_ERR_INVAL);\
      DBG(\
         if(is_same_tie(startpos)){\
            myself v(startpos);\
            if(endpos.is_valid()){\
               for(;; ++v)\
                  if(v == endpos)\
                     break;\
                  else if(!v || v == *this)\
                     return su_ERR_INVAL;\
            }else{\
               for(; v; ++v)\
               if(v == *this)\
                  return su_ERR_INVAL;\
            }\
         }\
      )\
      return m_view.insert_range(su__VIEW_DOWNCAST(startpos).m_view,\
         su__VIEW_DOWNCAST(endpos).m_view);\
   }\
   \
   su__VIEW_NAME &remove(base &endpos){\
      ASSERT_RET(is_valid(), *this);\
      ASSERT_RET(!endpos.is_valid() || is_same_tie(endpos), *this);\
      (void)m_view.remove_range(su__VIEW_DOWNCAST(endpos).m_view);\
      return *this;\
   }\
/*}*/

#define su__VIEW_IMPL_NONASSOC_CONST

#define su__VIEW_IMPL_ASSOC /*{*/\
   const_key_ptr key(void) const{\
      ASSERT_RET(is_valid(), NIL);\
      return key_type_traits::to_const_ptr(m_view.key());\
   }\
/*}*/

#define su__VIEW_IMPL_ASSOC_NONCONST
#define su__VIEW_IMPL_ASSOC_CONST

#define su__VIEW_IMPL_UNIDIR
#define su__VIEW_IMPL_UNIDIR_NONCONST

#define su__VIEW_IMPL_UNIDIR_CONST /*{*/\
   /* (We need to cast away the 'const', but it is preserved logically..) */\
   template<class TCOLL> su__VIEW_NAME &begin(TCOLL const &tc){\
      (void)m_view.tie(S(base_coll_type&,C(TCOLL&,tc))).begin();\
      return *this;\
   }\
/*}*/

#define su__VIEW_IMPL_UNIDIR_NONASSOC /*{*/\
   /* is_valid() must be tested thereafter */\
   su__VIEW_NAME &go_to(uz off){\
      ASSERT_RET(is_tied(), *this);\
      (void)m_view.go_to(off);\
      return *this;\
   }\
   \
   boole find(const_ptr dat, boole byptr=FAL0){\
      ASSERT_RET(is_tied(), (invalidate(), FAL0));\
      /* FIXME toolbox assert if !byptr */
      return m_view.find(type_traits::to_const_vptr(dat), byptr);\
   }\
/*}*/

#define su__VIEW_IMPL_UNIDIR_ASSOC /*{*/\
   boole find(const_key_ptr key){\
      ASSERT_RET(is_tied(), (invalidate(), FAL0));\
      return m_view.find(key_type_traits::to_const_vptr(key));\
   }\
/*}*/

#define su__VIEW_IMPL_BIDIR /*{*/\
   su__VIEW_NAME &end(void){\
      ASSERT_RET(is_tied(), *this);\
      (void)m_view.end();\
      return *this;\
   }\
   \
   boole has_last(void) const{\
      ASSERT_RET(is_valid(), FAL0);\
      return m_view.has_last();\
   }\
   su__VIEW_NAME &last(void){\
      ASSERT_RET(is_valid(), *this);\
      (void)m_view.last();\
      return *this;\
   }\
   su__VIEW_NAME &operator--(void) {return last();}\
/*}*/

#define su__VIEW_IMPL_BIDIR_NONCONST /*{*/\
   template<class TCOLL> su__VIEW_NAME &end(TCOLL &tc){\
      (void)m_view.tie(S(base_coll_type&,tc)).end();\
      return *this;\
   }\
/*}*/

#define su__VIEW_IMPL_BIDIR_CONST /*{*/\
   /* (We need to cast away the 'const', but it is preserved logically..) */\
   template<class TCOLL> su__VIEW_NAME &end(TCOLL const &tc){\
      (void)m_view.tie(S(base_coll_type&,C(TCOLL&,tc))).end();\
      return *this;\
   }\
/*}*/

#define su__VIEW_IMPL_BIDIR_NONASSOC /*{*/\
   boole rfind(const_ptr dat, boole byptr=FAL0){\
      ASSERT_RET(is_tied(), (invalidate(), FAL0));\
      /* FIXME toolbox assert if !byptr */
      return m_view.rfind(type_traits::to_const_vptr(dat), byptr);\
   }\
/*}*/

#define su__VIEW_IMPL_BIDIR_ASSOC

#define su__VIEW_IMPL_RANDOM
#define su__VIEW_IMPL_RANDOM_NONCONST
#define su__VIEW_IMPL_RANDOM_CONST

#define su__VIEW_IMPL_RANDOM_NONASSOC /*{*/\
   /* is_valid() must be tested thereafter */\
   su__VIEW_NAME &go_around(sz reloff){\
      ASSERT_RET(is_valid(), *this);\
      return m_view.go_around(reloff);\
   }\
   su__VIEW_NAME &operator+=(sz reloff) {return go_around(reloff);}\
   su__VIEW_NAME &operator-=(sz reloff) {return go_around(-reloff);}\
   su__VIEW_NAME operator+(sz reloff) const{\
      su__VIEW_NAME rv(*this);\
      ASSERT_RET(is_valid(), rv);\
      return (rv += reloff);\
   }\
   su__VIEW_NAME operator-(sz reloff) const{\
      su__VIEW_NAME rv(*this);\
      ASSERT_RET(is_valid(), rv);\
      return (rv -= reloff);\
   }\
   \
   boole operator<(base const &t) const {return (base::compare(t) < 0);}\
   boole operator>(base const &t) const {return (base::compare(t) > 0);}\
   boole operator<=(base const &t) const {return (base::compare(t) <= 0);}\
   boole operator>=(base const &t) const {return (base::compare(t) >= 0);}\
/*}*/

#define su__VIEW_IMPL_RANDOM_ASSOC

#define su__VIEW_IMPL_END };

// Puuuh.  Let us go!

#undef su__VIEW_CATEGORY
#define su__VIEW_CATEGORY view_category_non_assoc

#undef su__VIEW_TYPE
#define su__VIEW_TYPE view_type_unidir

#undef su__VIEW_NAME
#undef su__NONCONST_VIEW_NAME
#define su__VIEW_NAME view_unidir
#define su__NONCONST_VIEW_NAME view_unidir
su__VIEW_IMPL_START
   su__VIEW_IMPL_NONCONST
   su__VIEW_IMPL_NONASSOC
   su__VIEW_IMPL_NONASSOC_NONCONST
   su__VIEW_IMPL_UNIDIR
   su__VIEW_IMPL_UNIDIR_NONCONST
   su__VIEW_IMPL_UNIDIR_NONASSOC
su__VIEW_IMPL_END

#undef su__VIEW_NAME
#define su__VIEW_NAME const_view_unidir
su__VIEW_IMPL_START
   su__VIEW_IMPL_CONST
   su__VIEW_IMPL_NONASSOC
   su__VIEW_IMPL_NONASSOC_CONST
   su__VIEW_IMPL_UNIDIR
   su__VIEW_IMPL_UNIDIR_CONST
   su__VIEW_IMPL_UNIDIR_NONASSOC
su__VIEW_IMPL_END

#undef su__VIEW_TYPE
#define su__VIEW_TYPE view_type_bidir

#undef su__VIEW_NAME
#undef su__NONCONST_VIEW_NAME
#define su__VIEW_NAME view_bidir
#define su__NONCONST_VIEW_NAME view_bidir
su__VIEW_IMPL_START
   su__VIEW_IMPL_NONCONST
   su__VIEW_IMPL_NONASSOC
   su__VIEW_IMPL_NONASSOC_NONCONST
   su__VIEW_IMPL_UNIDIR
   su__VIEW_IMPL_UNIDIR_NONCONST
   su__VIEW_IMPL_UNIDIR_NONASSOC
   su__VIEW_IMPL_BIDIR
   su__VIEW_IMPL_BIDIR_NONCONST
   su__VIEW_IMPL_BIDIR_NONASSOC
su__VIEW_IMPL_END

#undef su__VIEW_NAME
#define su__VIEW_NAME const_view_bidir
su__VIEW_IMPL_START
   su__VIEW_IMPL_CONST
   su__VIEW_IMPL_NONASSOC
   su__VIEW_IMPL_NONASSOC_CONST
   su__VIEW_IMPL_UNIDIR
   su__VIEW_IMPL_UNIDIR_CONST
   su__VIEW_IMPL_UNIDIR_NONASSOC
   su__VIEW_IMPL_BIDIR
   su__VIEW_IMPL_BIDIR_CONST
   su__VIEW_IMPL_BIDIR_NONASSOC
su__VIEW_IMPL_END

#undef su__VIEW_TYPE
#define su__VIEW_TYPE view_type_random

#undef su__VIEW_NAME
#undef su__NONCONST_VIEW_NAME
#define su__VIEW_NAME view_random
#define su__NONCONST_VIEW_NAME view_random
su__VIEW_IMPL_START
   su__VIEW_IMPL_NONCONST
   su__VIEW_IMPL_NONASSOC
   su__VIEW_IMPL_NONASSOC_NONCONST
   su__VIEW_IMPL_UNIDIR
   su__VIEW_IMPL_UNIDIR_NONCONST
   su__VIEW_IMPL_UNIDIR_NONASSOC
   su__VIEW_IMPL_BIDIR
   su__VIEW_IMPL_BIDIR_NONCONST
   su__VIEW_IMPL_BIDIR_NONASSOC
   su__VIEW_IMPL_RANDOM
   su__VIEW_IMPL_RANDOM_NONCONST
   su__VIEW_IMPL_RANDOM_NONASSOC
su__VIEW_IMPL_END

#undef su__VIEW_NAME
#define su__VIEW_NAME const_view_random
su__VIEW_IMPL_START
   su__VIEW_IMPL_CONST
   su__VIEW_IMPL_NONASSOC
   su__VIEW_IMPL_NONASSOC_CONST
   su__VIEW_IMPL_UNIDIR
   su__VIEW_IMPL_UNIDIR_CONST
   su__VIEW_IMPL_UNIDIR_NONASSOC
   su__VIEW_IMPL_BIDIR
   su__VIEW_IMPL_BIDIR_CONST
   su__VIEW_IMPL_BIDIR_NONASSOC
   su__VIEW_IMPL_RANDOM
   su__VIEW_IMPL_RANDOM_CONST
   su__VIEW_IMPL_RANDOM_NONASSOC
su__VIEW_IMPL_END

#undef su__VIEW_CATEGORY
#define su__VIEW_CATEGORY view_category_assoc

#undef su__VIEW_TYPE
#define su__VIEW_TYPE view_type_assoc_unidir

#undef su__VIEW_NAME
#undef su__NONCONST_VIEW_NAME
#define su__VIEW_NAME view_assoc_unidir
#define su__NONCONST_VIEW_NAME view_assoc_unidir
su__VIEW_IMPL_START
   su__VIEW_IMPL_NONCONST
   su__VIEW_IMPL_ASSOC
   su__VIEW_IMPL_ASSOC_NONCONST
   su__VIEW_IMPL_UNIDIR
   su__VIEW_IMPL_UNIDIR_NONCONST
   su__VIEW_IMPL_UNIDIR_ASSOC
su__VIEW_IMPL_END

#undef su__VIEW_NAME
#define su__VIEW_NAME const_view_assoc_unidir
su__VIEW_IMPL_START
   su__VIEW_IMPL_CONST
   su__VIEW_IMPL_ASSOC
   su__VIEW_IMPL_ASSOC_CONST
   su__VIEW_IMPL_UNIDIR
   su__VIEW_IMPL_UNIDIR_CONST
   su__VIEW_IMPL_UNIDIR_ASSOC
su__VIEW_IMPL_END

#undef su__VIEW_TYPE
#define su__VIEW_TYPE view_type_assoc_bidir

#undef su__VIEW_NAME
#undef su__NONCONST_VIEW_NAME
#define su__VIEW_NAME view_assoc_bidir
#define su__NONCONST_VIEW_NAME view_assoc_bidir
su__VIEW_IMPL_START
   su__VIEW_IMPL_NONCONST
   su__VIEW_IMPL_ASSOC
   su__VIEW_IMPL_ASSOC_NONCONST
   su__VIEW_IMPL_UNIDIR
   su__VIEW_IMPL_UNIDIR_NONCONST
   su__VIEW_IMPL_UNIDIR_ASSOC
   su__VIEW_IMPL_BIDIR
   su__VIEW_IMPL_BIDIR_NONCONST
   su__VIEW_IMPL_BIDIR_ASSOC
su__VIEW_IMPL_END

#undef su__VIEW_NAME
#define su__VIEW_NAME const_view_assoc_bidir
su__VIEW_IMPL_START
   su__VIEW_IMPL_CONST
   su__VIEW_IMPL_ASSOC
   su__VIEW_IMPL_ASSOC_CONST
   su__VIEW_IMPL_UNIDIR
   su__VIEW_IMPL_UNIDIR_CONST
   su__VIEW_IMPL_UNIDIR_ASSOC
   su__VIEW_IMPL_BIDIR
   su__VIEW_IMPL_BIDIR_CONST
   su__VIEW_IMPL_BIDIR_ASSOC
   su__VIEW_IMPL_RANDOM
   su__VIEW_IMPL_RANDOM_CONST
   su__VIEW_IMPL_RANDOM_ASSOC
su__VIEW_IMPL_END

// Cleanup

#undef su__VIEW_DOWNCAST

#undef su__VIEW_IMPL_START
#undef su__VIEW_IMPL_NONCONST
#undef su__VIEW_IMPL_CONST
#undef su__VIEW_IMPL_NONASSOC
#undef su__VIEW_IMPL_NONASSOC_NONCONST
#undef su__VIEW_IMPL_NONASSOC_CONST
#undef su__VIEW_IMPL_ASSOC
#undef su__VIEW_IMPL_ASSOC_NONCONST
#undef su__VIEW_IMPL_ASSOC_CONST
#undef su__VIEW_IMPL_UNIDIR
#undef su__VIEW_IMPL_UNIDIR_NONCONST
#undef su__VIEW_IMPL_UNIDIR_CONST
#undef su__VIEW_IMPL_UNIDIR_NONASSOC
#undef su__VIEW_IMPL_UNIDIR_ASSOC
#undef su__VIEW_IMPL_BIDIR
#undef su__VIEW_IMPL_BIDIR_NONCONST
#undef su__VIEW_IMPL_BIDIR_CONST
#undef su__VIEW_IMPL_BIDIR_NONASSOC
#undef su__VIEW_IMPL_BIDIR_ASSOC
#undef su__VIEW_IMPL_RANDOM
#undef su__VIEW_IMPL_RANDOM_NONCONST
#undef su__VIEW_IMPL_RANDOM_CONST
#undef su__VIEW_IMPL_RANDOM_NONASSOC
#undef su__VIEW_IMPL_RANDOM_ASSOC
#undef su__VIEW_IMPL_END

#undef su__VIEW_CATEGORY
#undef su__VIEW_TYPE
#undef su__VIEW_NAME
#undef su__NONCONST_VIEW_NAME

NSPC_END(su)
#include <su/code-ou.h>
#endif /* !C_LANG */
#endif /* su_VIEW_H */
/* s-it-mode */
