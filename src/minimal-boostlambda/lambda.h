#ifndef LAMBDA_H
#define LAMBDA_H

#include "boost/tuple/tuple.hpp"

namespace boost {
namespace lambda {

#define CALL_TEMPLATE_ARGS class A, class B, class C, class Env
#define CALL_FORMAL_ARGS A& a, B& b, C& c, Env& env
#define CALL_ACTUAL_ARGS a, b, c, env
#define CALL_ACTUAL_ARGS_NO_ENV a, b, c
#define CALL_REFERENCE_TYPES A, B, C, Env&
#define CALL_PLAIN_TYPES A, B, C, Env

#define LAMBDA_DISABLE_IF_ARRAY1(A1, R1) typename R1::type
#define LAMBDA_DISABLE_IF_ARRAY2(A1, A2, R1, R2) typename R1, R2::type
#define LAMBDA_DISABLE_IF_ARRAY3(A1, A2, A3, R1, R2, R3) typename R1, R2, R3::type

template<class A1, class A2, class A3, class A4>
void do_nothing(A1&, A2&, A3&, A4&) {
}

#define CALL_USE_ARGS \
do_nothing(a, b, c, env)

template<class Base>
class lambda_functor;
template<class Act, class Args>
class lambda_functor_base;

struct null_type {};

static const null_type constant_null_type = null_type();
#define cnull_type() constant_null_type

enum {
    NONE = 0x00, // Notice we are using bits as flags here.
    FIRST = 0x01,
    SECOND = 0x02,
    THIRD = 0x04,
    EXCEPTION = 0x08,
    RETHROW = 0x10
};

template<bool If, class Then, class Else> struct IF {
    typedef Then RET;
};

template<class Then, class Else> struct IF<false, Then, Else> {
    typedef Else RET;
};

template<class T1, class T2>
struct parameter_traits_ {
    typedef T2 type;
};

template<class T>
struct const_copy_argument {
typedef typename
    parameter_traits_<
        T,
        typename IF<boost::is_function<T>::value, T&, const T>::RET
    >::type type;
};

// returns a reference to the element of tuple T
template<int N, class T> struct tuple_element_as_reference {
typedef typename
    boost::tuples::access_traits<
        typename boost::tuples::element<N, T>::type
    >::non_const_type type;
};
template<int N, class Tuple> struct get_element_or_null_type {
typedef typename
    tuple_element_as_reference<N, Tuple>::type type;
};
template<int N> struct get_element_or_null_type<N, null_type> {
    typedef null_type type;
};

template<int I> struct placeholder;

template<> struct placeholder<FIRST> {
    template<class SigArgs> struct sig {
        typedef typename get_element_or_null_type<0, SigArgs>::type type;
    };

    template<class RET, CALL_TEMPLATE_ARGS>
    RET call(CALL_FORMAL_ARGS) const {
        BOOST_STATIC_ASSERT(boost::is_reference<RET>::value);
        CALL_USE_ARGS; // does nothing, prevents warnings for unused args
        return a;
    }
};

template<> struct placeholder<SECOND> {

    template<class SigArgs> struct sig {
        typedef typename get_element_or_null_type<1, SigArgs>::type type;
    };

    template<class RET, CALL_TEMPLATE_ARGS>
    RET call(CALL_FORMAL_ARGS) const {CALL_USE_ARGS; return b;}
};

template<> struct placeholder<THIRD> {

    template<class SigArgs> struct sig {
        typedef typename get_element_or_null_type<2, SigArgs>::type type;
    };

    template<class RET, CALL_TEMPLATE_ARGS>
    RET call(CALL_FORMAL_ARGS) const {CALL_USE_ARGS; return c;}
};

template<> struct placeholder<EXCEPTION> {

    template<class SigArgs> struct sig {
        typedef typename get_element_or_null_type<3, SigArgs>::type type;
    };

    template<class RET, CALL_TEMPLATE_ARGS>
    RET call(CALL_FORMAL_ARGS) const {CALL_USE_ARGS; return env;}
};

// select functions -------------------------------
template<class Any, CALL_TEMPLATE_ARGS>
inline Any& select(Any& any, CALL_FORMAL_ARGS) {CALL_USE_ARGS; return any;}

template<class Arg, CALL_TEMPLATE_ARGS>
inline typename Arg::template sig<tuple<CALL_REFERENCE_TYPES> >::type
select ( const lambda_functor<Arg>& op, CALL_FORMAL_ARGS ) {
    return op.template call<
    typename Arg::template sig<tuple<CALL_REFERENCE_TYPES> >::type
>    (CALL_ACTUAL_ARGS);
}
template<class Arg, CALL_TEMPLATE_ARGS>
inline typename Arg::template sig<tuple<CALL_REFERENCE_TYPES> >::type
select ( lambda_functor<Arg>& op, CALL_FORMAL_ARGS) {
    return op.template call<
    typename Arg::template sig<tuple<CALL_REFERENCE_TYPES> >::type
>    (CALL_ACTUAL_ARGS);
}

class plus_action {};
class minus_action {};
class multiply_action {};
class divide_action {};

#define LAMBDA_BINARY_ACTION(SYMBOL, ACTION_CLASS)                        \
template<class Args>                                                      \
class lambda_functor_base<ACTION_CLASS, Args> {                           \
public:                                                                   \
  Args args;                                                              \
public:                                                                   \
  explicit lambda_functor_base(const Args& a) : args(a) {}                \
                                                                          \
  template<class RET, CALL_TEMPLATE_ARGS>                                 \
  RET call(CALL_FORMAL_ARGS) const {                                      \
    return select(boost::tuples::get<0>(args), CALL_ACTUAL_ARGS)          \
           SYMBOL                                                         \
           select(boost::tuples::get<1>(args), CALL_ACTUAL_ARGS);         \
  }                                                                       \
  template<class SigArgs> struct sig {                                    \
    typedef typename                                                      \
        boost::tuples::element<0,SigArgs>::type type;                     \
  };                                                                      \
};

LAMBDA_BINARY_ACTION(+,plus_action)
LAMBDA_BINARY_ACTION(-,minus_action)
LAMBDA_BINARY_ACTION(*,multiply_action)
LAMBDA_BINARY_ACTION(/,divide_action)

template<class T>
class lambda_functor: public T {
public:
    typedef T inherited;

    lambda_functor() {}
    lambda_functor(const lambda_functor& l) : inherited(l) {}
    lambda_functor(const T& t) :
        inherited(t) {
    }

      template<class A>
      typename inherited::template sig<tuple<A> >::type
      operator()(A& a) const {
        return inherited::template call<
          typename inherited::template sig<tuple<A> >::type
        >(a, cnull_type(), cnull_type(), cnull_type());
      }

      template<class A>
      LAMBDA_DISABLE_IF_ARRAY1(A, inherited::template sig<tuple<A> >)
      operator()(A const& a) const {
        return inherited::template call<
          typename inherited::template sig<tuple<A> >::type
        >(a, cnull_type(), cnull_type(), cnull_type());
      }

    template<class A, class B>
    typename inherited::template sig<tuple<A, B> >::type
    operator()(A& a, B& b) const {
        return inherited::template call<
            typename inherited::template sig<tuple<A, B> >::type
        >(a, b, cnull_type(), cnull_type());
    }

    template<class A, class B>
    LAMBDA_DISABLE_IF_ARRAY2(A, B, inherited::template sig<tuple<A, B> >)
    operator()(A const& a, B& b) const {
        return inherited::template call<
            typename inherited::template sig<tuple<A, B> >::type
        >(a, b, cnull_type(), cnull_type());
    }

    template<class A, class B>
    LAMBDA_DISABLE_IF_ARRAY2(A, B, inherited::template sig<tuple<A, B> >)
    operator()(A& a, B const& b) const {
        return inherited::template call<
            typename inherited::template sig<tuple<A, B> >::type
        >(a, b, cnull_type(), cnull_type());
    }

    template<class A, class B>
    LAMBDA_DISABLE_IF_ARRAY2(A, B, inherited::template sig<tuple<A, B> >)
    operator()(A const& a, B const& b) const {
        return inherited::template call<
            typename inherited::template sig<tuple<A, B> >::type
        >(a, b, cnull_type(), cnull_type());
    }

    template<class A, class B, class C>
    typename inherited::template sig<tuple<A, B, C> >::type
    operator()(A& a, B& b, C& c) const
    {
        return inherited::template call<
            typename inherited::template sig<tuple<A, B, C> >::type
        >(a, b, c, cnull_type());
    }

    template<class A, class B, class C>
    LAMBDA_DISABLE_IF_ARRAY3(A, B, C, inherited::template sig<tuple<A , B , C> >)
    operator()(A const& a, B const& b, C const& c) const
    {
        return inherited::template call<
            typename inherited::template sig<tuple<A , B , C> >::type
        >(a, b, c, cnull_type());
    }
};

#define LAMBDA_BE1(OPER_NAME, ACTION, CONSTA, CONSTB, CONVERSION)            \
template<class Arg, class B>                                                 \
inline const                                                                 \
lambda_functor<                                                              \
  lambda_functor_base<                                                       \
    ACTION,                                                                  \
    tuple<lambda_functor<Arg>, typename const_copy_argument <CONSTB>::type>  \
  >                                                                          \
>                                                                            \
OPER_NAME (const lambda_functor<Arg>& a, CONSTB& b) {                        \
  return                                                                     \
    lambda_functor_base<                                                     \
      ACTION,                                                                \
      tuple<lambda_functor<Arg>, typename const_copy_argument <CONSTB>::type>\
    >                                                                        \
   (tuple<lambda_functor<Arg>, typename const_copy_argument <CONSTB>::type>(a, b)); \
}

#define LAMBDA_BE2(OPER_NAME, ACTION, CONSTA, CONSTB, CONVERSION)            \
template<class A, class Arg>                                                 \
inline const                                                                 \
lambda_functor<                                                              \
  lambda_functor_base<                                                       \
    ACTION,                                                                  \
    tuple<typename CONVERSION <CONSTA>::type, lambda_functor<Arg> >          \
  >                                                                          \
>                                                                            \
OPER_NAME (CONSTA& a, const lambda_functor<Arg>& b) {                        \
  return                                                                     \
    lambda_functor_base<                                                     \
      ACTION,                                                                \
      tuple<typename CONVERSION <CONSTA>::type, lambda_functor<Arg> >        \
    >                                                                        \
  (tuple<typename CONVERSION <CONSTA>::type, lambda_functor<Arg> >(a, b));   \
}

#define LAMBDA_BE3(OPER_NAME, ACTION, CONSTA, CONSTB, CONVERSION)          \
template<class ArgA, class ArgB>                                           \
inline const                                                               \
lambda_functor<                                                            \
  lambda_functor_base<                                                     \
    ACTION,                                                                \
    tuple<lambda_functor<ArgA>, lambda_functor<ArgB> >                     \
  >                                                                        \
>                                                                          \
OPER_NAME (const lambda_functor<ArgA>& a, const lambda_functor<ArgB>& b) { \
  return                                                                   \
    lambda_functor_base<                                                   \
      ACTION,                                                              \
      tuple<lambda_functor<ArgA>, lambda_functor<ArgB> >                   \
    >                                                                      \
  (tuple<lambda_functor<ArgA>, lambda_functor<ArgB> >(a, b));              \
}

#define LAMBDA_BE(OPER_NAME, ACTION, CONSTA, CONSTB, CONST_CONVERSION)     \
    LAMBDA_BE1(OPER_NAME, ACTION, CONSTA, CONSTB, CONST_CONVERSION)        \
    LAMBDA_BE2(OPER_NAME, ACTION, CONSTA, CONSTB, CONST_CONVERSION)        \
    LAMBDA_BE3(OPER_NAME, ACTION, CONSTA, CONSTB, CONST_CONVERSION)

LAMBDA_BE(operator+, plus_action,const A,const B,const_copy_argument)
LAMBDA_BE(operator-, minus_action,const A,const B,const_copy_argument)
LAMBDA_BE(operator*, multiply_action,const A,const B,const_copy_argument)
LAMBDA_BE(operator/, divide_action,const A,const B,const_copy_argument)

typedef const lambda_functor<placeholder<FIRST> > placeholder1_type;
typedef const lambda_functor<placeholder<SECOND> > placeholder2_type;
typedef const lambda_functor<placeholder<THIRD> > placeholder3_type;

placeholder1_type free1 = placeholder1_type();
placeholder2_type free2 = placeholder2_type();
placeholder3_type free3 = placeholder3_type();

placeholder1_type& _1 = free1;
placeholder2_type& _2 = free2;
placeholder3_type& _3 = free3;

} //lambda
} //boost

#endif //LAMBDA_H
