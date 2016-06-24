#ifndef LAMBDA_LAMBDA_H
#define LAMBDA_LAMBDA_H

#include <type_traits>

template<class A1, class A2> void do_nothing(const A1 &, const A2&) {}
#define CALL_USE_ARGS do_nothing(a, b)  // suppress -Wunused-parameter

template<class Action, class ArgA, class ArgB>
struct lambda_binary_functor;
class assignment_action;
class subscript_action;

template<class T>
class lambda_functor: public T {
public:
    typedef T inherited;

    lambda_functor() = default;
    lambda_functor(const lambda_functor &l): inherited(l) {}
    lambda_functor(const T &t): inherited(t) {}

    template<class A>
    auto operator()(A &a) const -> decltype(inherited::template call(a, nullptr)) {
        return inherited::template call(a, nullptr);
    }
    template<class A>
    auto operator()(const A &a) const -> decltype(inherited::template call(a, nullptr)) {
        return inherited::template call(a, nullptr);
    }
    template<class A, class B>
    auto operator()(A &a, B &b) const -> decltype(inherited::template call(a, b)) {
        return inherited::template call(a, b);
    }
    template<class A, class B>
    auto operator()(A &a, const B &b) const -> decltype(inherited::template call(a, b)) {
        return inherited::template call(a, b);
    }
    template<class A, class B>
    auto operator()(const A &a, B &b) const -> decltype(inherited::template call(a, b)) {
        return inherited::template call(a, b);
    }
    template<class A, class B>
    auto operator()(const A &a, const B &b) const -> decltype(inherited::template call(a, b)) {
        return inherited::template call(a, b);
    }
    template<class A>
    lambda_functor<lambda_binary_functor<assignment_action, const lambda_functor<T>, const A> >
    operator = (const A &a) const {
        return lambda_binary_functor<assignment_action, const lambda_functor<T>, const A>(*this, a);
    }
    template<class A>
    lambda_functor<lambda_binary_functor<subscript_action, const lambda_functor<T>, const A> >
    operator [] (const A &a) const {
        return lambda_binary_functor<subscript_action, const lambda_functor<T>, const A>(*this, a);
    }
};

#define EXPRESSION(EXP)                                                                                             \
template<class A, class B> auto call(A &a, B &b) const -> decltype(EXP) { CALL_USE_ARGS; return EXP; }              \
template<class A, class B> auto call(const A &a, B &b) const -> decltype(EXP) { CALL_USE_ARGS; return EXP; }        \
template<class A, class B> auto call(A &a, const B &b) const -> decltype(EXP) { CALL_USE_ARGS; return EXP; }        \
template<class A, class B> auto call(const A &a, const B &b) const -> decltype(EXP) { CALL_USE_ARGS; return EXP; }  \

template<int I> struct placeholder;
#define LAMBDA_PLACEHOLDER(POSITION, RETURN_ARGUMENT, NAME) \
template<> struct placeholder<POSITION> {                   \
    EXPRESSION((RETURN_ARGUMENT))                           \
};                                                          \
const auto NAME = lambda_functor<placeholder<POSITION> >();
LAMBDA_PLACEHOLDER(0, a, _1)
LAMBDA_PLACEHOLDER(1, b, _2)


template<class Any, class A, class B>
inline Any &select(Any &any, const A &a, const B&b) {
    CALL_USE_ARGS;
    return any;
}
template<class Arg, class A, class B>
inline auto select(const lambda_functor<Arg> &op, A &a, B &b) -> decltype(op.template call(a, b)) {
    return op.template call(a, b);
}
template<class Arg, class A, class B>
inline auto select(const lambda_functor<Arg> &op, const A &a, B &b) -> decltype(op.template call(a, b)) {
    return op.template call(a, b);
}
template<class Arg, class A, class B>
inline auto select(const lambda_functor<Arg> &op, A &a, const B &b) -> decltype(op.template call(a, b)) {
    return op.template call(a, b);
}
template<class Arg, class A, class B>
inline auto select(const lambda_functor<Arg> &op, const A &a, const B &b) -> decltype(op.template call(a, b)) {
    return op.template call(a, b);
}

template<class Action, class ArgA, class ArgB>
struct lambda_binary_functor;
#define LAMBDA_BINARY_ACTION_CLASS(SYMBOL, ACTION_CLASS)                        \
class ACTION_CLASS{};                                                           \
template<class ArgA, class ArgB>                                                \
struct lambda_binary_functor<ACTION_CLASS, ArgA, ArgB> {                        \
    ArgA &arga;                                                                 \
    ArgB &argb;                                                                 \
    lambda_binary_functor(ArgA &arga, ArgB &argb): arga(arga), argb(argb) {}    \
    EXPRESSION(select(arga, a, b) SYMBOL select(argb, a, b))                    \
};
#define LAMBDA_BINARY_ACTION_OPERATOR(SYMBOL, ACTION_CLASS)                                                     \
template<class Arg, class B>                                                                                    \
inline lambda_functor<lambda_binary_functor<ACTION_CLASS, const lambda_functor<Arg>, B> >                       \
operator SYMBOL (const lambda_functor<Arg> &a, B &b) {                                                          \
    return lambda_binary_functor<ACTION_CLASS, const lambda_functor<Arg>, B>(a, b);                             \
}                                                                                                               \
template<class Arg, class B>                                                                                    \
inline lambda_functor<lambda_binary_functor<ACTION_CLASS, const lambda_functor<Arg>, const B> >                 \
operator SYMBOL (const lambda_functor<Arg> &a, const B &b) {                                                    \
    return lambda_binary_functor<ACTION_CLASS, const lambda_functor<Arg>, const B>(a, b);                       \
}                                                                                                               \
template<class Arg, class A>                                                                                    \
inline lambda_functor<lambda_binary_functor<ACTION_CLASS, A, const lambda_functor<Arg> > >                      \
operator SYMBOL (A &a, const lambda_functor<Arg> &b) {                                                          \
    return lambda_binary_functor<ACTION_CLASS, A, const lambda_functor<Arg> >(a, b);                            \
}                                                                                                               \
template<class Arg, class A>                                                                                    \
inline lambda_functor<lambda_binary_functor<ACTION_CLASS, const A, const lambda_functor<Arg> > >                \
operator SYMBOL (const A &a, const lambda_functor<Arg> &b) {                                                    \
    return lambda_binary_functor<ACTION_CLASS, const A, const lambda_functor<Arg> >(a, b);                      \
}                                                                                                               \
template<class A, class B>                                                                                      \
inline lambda_functor<lambda_binary_functor<ACTION_CLASS, const lambda_functor<A>, const lambda_functor<B> > >  \
operator SYMBOL (const lambda_functor<A> &a, const lambda_functor<B> &b) {                                      \
    return lambda_binary_functor<ACTION_CLASS, const lambda_functor<A>, const lambda_functor<B> >(a, b);        \
}
#define LAMBDA_BINARY_ACTION(SYMBOL, ACTION_CLASS)      \
    LAMBDA_BINARY_ACTION_CLASS(SYMBOL, ACTION_CLASS)    \
    LAMBDA_BINARY_ACTION_OPERATOR(SYMBOL, ACTION_CLASS)

template<class Action, class Arg>
struct lambda_prefix_unary_action{};
#define LAMBDA_PREFIX_UNARY_ACTION(SYMBOL, ACTION_CLASS)                                    \
class ACTION_CLASS{};                                                                       \
template<class Arg>                                                                         \
struct lambda_prefix_unary_action<ACTION_CLASS, Arg> {                                      \
    Arg &arg;                                                                               \
    lambda_prefix_unary_action(Arg &arg): arg(arg) {}                                       \
    EXPRESSION(SYMBOL select(arg, a, b))                                                    \
};                                                                                          \
template<class A>                                                                           \
inline lambda_functor<lambda_prefix_unary_action<ACTION_CLASS, const lambda_functor<A> > >  \
operator SYMBOL (const lambda_functor<A> &a) {                                              \
    return lambda_prefix_unary_action<ACTION_CLASS, const lambda_functor<A> >(a);           \
}

template<class Action, class Arg>
struct lambda_postfix_unary_action{};
#define LAMBDA_POSTFIX_UNARY_ACTION(SYMBOL, ACTION_CLASS)                                   \
class ACTION_CLASS{};                                                                       \
template<class Arg>                                                                         \
struct lambda_postfix_unary_action<ACTION_CLASS, Arg> {                                     \
    Arg &arg;                                                                               \
    lambda_postfix_unary_action(Arg &arg): arg(arg) {}                                      \
    EXPRESSION(select(arg, a, b) SYMBOL)                                                    \
};                                                                                          \
template<class A>                                                                           \
inline lambda_functor<lambda_postfix_unary_action<ACTION_CLASS, const lambda_functor<A> > > \
operator SYMBOL (const lambda_functor<A> &a, int) {                                         \
    return lambda_postfix_unary_action<ACTION_CLASS, const lambda_functor<A> >(a);          \
}

LAMBDA_BINARY_ACTION(+,   plus_action)
LAMBDA_BINARY_ACTION(-,   minus_action)
LAMBDA_BINARY_ACTION(*,   multiply_action)
LAMBDA_BINARY_ACTION(/,   divide_action)
LAMBDA_BINARY_ACTION(%,   remainder_action)
LAMBDA_BINARY_ACTION(<<,  leftshift_action)
LAMBDA_BINARY_ACTION(>>,  rightshift_action)
LAMBDA_BINARY_ACTION(&,   bitwise_and_action)
LAMBDA_BINARY_ACTION(|,   bitwise_or_action)
LAMBDA_BINARY_ACTION(^,   bitwise_xor_action)
LAMBDA_BINARY_ACTION(&&,  logical_and_action)
LAMBDA_BINARY_ACTION(||,  logicla_or_action)
LAMBDA_BINARY_ACTION(<,   less_action)
LAMBDA_BINARY_ACTION(>,   greater_action)
LAMBDA_BINARY_ACTION(<=,  lessorequal_action)
LAMBDA_BINARY_ACTION(>=,  greaterorequal_action)
LAMBDA_BINARY_ACTION(==,  equal_action)
LAMBDA_BINARY_ACTION(!=,  notequal_action)
LAMBDA_BINARY_ACTION(+=,  plus_assignment_action)
LAMBDA_BINARY_ACTION(-=,  minus_assignment_action)
LAMBDA_BINARY_ACTION(*=,  multiply_assignment_action)
LAMBDA_BINARY_ACTION(/=,  divide_assignment_action)
LAMBDA_BINARY_ACTION(%=,  remainder_assignment_action)
LAMBDA_BINARY_ACTION(<<=, leftshift_assignment_action)
LAMBDA_BINARY_ACTION(>>=, rightshift_assignment_action)
LAMBDA_BINARY_ACTION(&=,  bitwise_and_assignment_action)
LAMBDA_BINARY_ACTION(|=,  bitwise_or_assignment_action)
LAMBDA_BINARY_ACTION(^=,  bitwise_xor_assignment_action)

#define COMMA ,     // Macro works not well for comma
class comma_action{};
template<class ArgA, class ArgB>
struct lambda_binary_functor<comma_action, ArgA, ArgB> {
    ArgA &arga;
    ArgB &argb;
    lambda_binary_functor(ArgA &arga, ArgB &argb): arga(arga), argb(argb) {}
    EXPRESSION(select(arga, a, b) COMMA select(argb, a, b))
};
LAMBDA_BINARY_ACTION_OPERATOR(COMMA, comma_action)

// Non-member
LAMBDA_BINARY_ACTION_CLASS(=, assignment_action)
class subscript_action{};
template<class ArgA, class ArgB>
struct lambda_binary_functor<subscript_action, ArgA, ArgB> {
    ArgA &arga;
    ArgB &argb;
    lambda_binary_functor(ArgA &arga, ArgB &argb): arga(arga), argb(argb) {}
    EXPRESSION(select(arga, a, b)[select(argb, a, b)])
};

LAMBDA_PREFIX_UNARY_ACTION(+,  unary_plus_action)
LAMBDA_PREFIX_UNARY_ACTION(-,  unary_minus_action)
LAMBDA_PREFIX_UNARY_ACTION(~,  bitwise_not_action)
LAMBDA_PREFIX_UNARY_ACTION(!,  logical_not_action)
LAMBDA_PREFIX_UNARY_ACTION(++, pre_increment_action)
LAMBDA_PREFIX_UNARY_ACTION(--, pre_decrement_action)
LAMBDA_PREFIX_UNARY_ACTION(*,  contentsof_action)
LAMBDA_PREFIX_UNARY_ACTION(&,  addressof_action)

LAMBDA_POSTFIX_UNARY_ACTION(++, post_increment_action)
LAMBDA_POSTFIX_UNARY_ACTION(--, post_decrement_action)

template<class Arg>
struct lambda_var_action {
    Arg &arg;
    lambda_var_action(Arg &arg): arg(arg) {}
    EXPRESSION(select(arg, a, b))
};
template<class A>
inline lambda_functor<lambda_var_action<A> > ref(A &a) { return lambda_var_action<A>(a); }
template<class A>
inline lambda_functor<lambda_var_action<const A> > ref(const A &a) { return lambda_var_action<const A>(a); }

#endif //LAMBDA_LAMBDA_H
