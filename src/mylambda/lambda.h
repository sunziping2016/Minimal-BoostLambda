#ifndef LAMBDA_LAMBDA2_H
#define LAMBDA_LAMBDA2_H

#include <utility>

template<class... Args> inline void do_nothing(Args &&...) {}  // suppress -Wunused-parameter
template<> inline void do_nothing() {};

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

    template<class... Args>
    auto operator()(Args &&... args) const -> decltype(inherited::call(std::forward<Args>(args)...)) {
        return inherited::template call(std::forward<Args>(args)...);
    }
    template<class A>
    lambda_functor<lambda_binary_functor<assignment_action, const lambda_functor<T>, A> >
    operator = (A &a) const {
        return lambda_binary_functor<assignment_action, const lambda_functor<T>, A>(*this, a);
    }
    template<class A>
    lambda_functor<lambda_binary_functor<assignment_action, const lambda_functor<T>, const A> >
    operator = (const A &a) const {
        return lambda_binary_functor<assignment_action, const lambda_functor<T>, const A>(*this, a);
    }
    template<class A>
    lambda_functor<lambda_binary_functor<subscript_action, const lambda_functor<T>, A> >
    operator [] (A &a) const {
        return lambda_binary_functor<subscript_action, const lambda_functor<T>, A>(*this, a);
    }
    template<class A>
    lambda_functor<lambda_binary_functor<subscript_action, const lambda_functor<T>, const A> >
    operator [] (const A &a) const {
        return lambda_binary_functor<subscript_action, const lambda_functor<T>, const A>(*this, a);
    }
};


template<int N>
struct placeholder {
    template<class Arg, class... Args>
    auto call(Arg &&arg, Args &&... args) const -> decltype(placeholder<N - 1>().call(std::forward<Args>(args)...)) {
        do_nothing(arg);
        return placeholder<N - 1>().call(std::forward<Args>(args)...);
    }
};
template<>
struct placeholder<0> {
    template<class Arg, class... Args>
    auto call(Arg &&arg, Args &&... args) const -> decltype(arg) {
        do_nothing(args...);
        return std::forward<Arg>(arg);
    }
};

#define LAMBDA_PLACEHOLDER(POSITION, NAME) const auto NAME = lambda_functor<placeholder<POSITION> >();

LAMBDA_PLACEHOLDER(0, _1)
LAMBDA_PLACEHOLDER(1, _2)
LAMBDA_PLACEHOLDER(2, _3)
LAMBDA_PLACEHOLDER(3, _4)
LAMBDA_PLACEHOLDER(4, _5)
LAMBDA_PLACEHOLDER(5, _6)
LAMBDA_PLACEHOLDER(6, _7)
LAMBDA_PLACEHOLDER(7, _8)
LAMBDA_PLACEHOLDER(8, _9)

template<class Any, class... Args>
inline auto select(Any &&any, Args &&... args) -> decltype(any) {
    do_nothing(args...);
    return std::forward<Any>(any);
}
template<class Arg, class... Args>
inline auto select(const lambda_functor<Arg> &op, Args &&... args) -> decltype(op.template call(std::forward<Args>(args)...)) {
    return op.template call(std::forward<Args>(args)...);
}

#define LAMBDA_FUNCTOR_EXPRESSION(...) template<class... Args> auto call(Args &&... args) const -> decltype(__VA_ARGS__) { return __VA_ARGS__; }

template<class Action, class ArgA, class ArgB>
struct lambda_binary_functor{};
#define LAMBDA_BINARY_ACTION_CLASS(SYMBOL, ACTION_CLASS)                                                                  \
class ACTION_CLASS{};                                                                                                     \
template<class ArgA, class ArgB>                                                                                          \
struct lambda_binary_functor<ACTION_CLASS, ArgA, ArgB> {                                                                  \
    ArgA &arga;                                                                                                           \
    ArgB &argb;                                                                                                           \
    lambda_binary_functor(ArgA &arga, ArgB &argb): arga(arga), argb(argb) {}                                              \
    LAMBDA_FUNCTOR_EXPRESSION(select(arga, std::forward<Args>(args)...) SYMBOL select(argb, std::forward<Args>(args)...)) \
};
#define LAMBDA_BINARY_ACTION_OPERATOR(SYMBOL, ACTION_CLASS)                                                    \
template<class Arg, class B>                                                                                   \
inline lambda_functor<lambda_binary_functor<ACTION_CLASS, const lambda_functor<Arg>, B> >                      \
operator SYMBOL (const lambda_functor<Arg> &a, B &b) {                                                         \
    return lambda_binary_functor<ACTION_CLASS, const lambda_functor<Arg>, B>(a, b);                            \
}                                                                                                              \
template<class Arg, class B>                                                                                   \
inline lambda_functor<lambda_binary_functor<ACTION_CLASS, const lambda_functor<Arg>, const B> >                \
operator SYMBOL (const lambda_functor<Arg> &a, const B &b) {                                                   \
    return lambda_binary_functor<ACTION_CLASS, const lambda_functor<Arg>, const B>(a, b);                      \
}                                                                                                              \
template<class Arg, class A>                                                                                   \
inline lambda_functor<lambda_binary_functor<ACTION_CLASS, A, const lambda_functor<Arg> > >                     \
operator SYMBOL (A &a, const lambda_functor<Arg> &b) {                                                         \
    return lambda_binary_functor<ACTION_CLASS, A, const lambda_functor<Arg> >(a, b);                           \
}                                                                                                              \
template<class Arg, class A>                                                                                   \
inline lambda_functor<lambda_binary_functor<ACTION_CLASS, const A, const lambda_functor<Arg> > >               \
operator SYMBOL (const A &a, const lambda_functor<Arg> &b) {                                                   \
    return lambda_binary_functor<ACTION_CLASS, const A, const lambda_functor<Arg> >(a, b);                     \
}                                                                                                              \
template<class A, class B>                                                                                     \
inline lambda_functor<lambda_binary_functor<ACTION_CLASS, const lambda_functor<A>, const lambda_functor<B> > > \
operator SYMBOL (const lambda_functor<A> &a, const lambda_functor<B> &b) {                                     \
    return lambda_binary_functor<ACTION_CLASS, const lambda_functor<A>, const lambda_functor<B> >(a, b);       \
}
#define LAMBDA_BINARY_ACTION(SYMBOL, ACTION_CLASS)      \
    LAMBDA_BINARY_ACTION_CLASS(SYMBOL, ACTION_CLASS)    \
    LAMBDA_BINARY_ACTION_OPERATOR(SYMBOL, ACTION_CLASS)

template<class Action, class Arg>
struct lambda_prefix_unary_functor{};
#define LAMBDA_PREFIX_UNARY_ACTION(SYMBOL, ACTION_CLASS)                                    \
class ACTION_CLASS{};                                                                       \
template<class Arg>                                                                         \
struct lambda_prefix_unary_functor<ACTION_CLASS, Arg> {                                     \
    Arg &arg;                                                                               \
    lambda_prefix_unary_functor(Arg &arg): arg(arg) {}                                      \
    LAMBDA_FUNCTOR_EXPRESSION(SYMBOL select(arg, std::forward<Args>(args)...))              \
};                                                                                          \
template<class A>                                                                           \
inline lambda_functor<lambda_prefix_unary_functor<ACTION_CLASS, const lambda_functor<A> > > \
operator SYMBOL (const lambda_functor<A> &a) {                                              \
    return lambda_prefix_unary_functor<ACTION_CLASS, const lambda_functor<A> >(a);          \
}

template<class Action, class Arg>
struct lambda_postfix_unary_functor{};
#define LAMBDA_POSTFIX_UNARY_ACTION(SYMBOL, ACTION_CLASS)                                    \
class ACTION_CLASS{};                                                                        \
template<class Arg>                                                                          \
struct lambda_postfix_unary_functor<ACTION_CLASS, Arg> {                                     \
    Arg &arg;                                                                                \
    lambda_postfix_unary_functor(Arg &arg): arg(arg) {}                                      \
    LAMBDA_FUNCTOR_EXPRESSION(select(arg, std::forward<Args>(args)...) SYMBOL)               \
};                                                                                           \
template<class A>                                                                            \
inline lambda_functor<lambda_postfix_unary_functor<ACTION_CLASS, const lambda_functor<A> > > \
operator SYMBOL (const lambda_functor<A> &a, int) {                                          \
    return lambda_postfix_unary_functor<ACTION_CLASS, const lambda_functor<A> >(a);          \
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
LAMBDA_BINARY_ACTION_CLASS(COMMA, comma_action)
LAMBDA_BINARY_ACTION_OPERATOR(COMMA, comma_action)

// Non-member
LAMBDA_BINARY_ACTION_CLASS(=, assignment_action)
class subscript_action{};
template<class ArgA, class ArgB>
struct lambda_binary_functor<subscript_action, ArgA, ArgB> {
    ArgA &arga;
    ArgB &argb;
    lambda_binary_functor(ArgA &arga, ArgB &argb): arga(arga), argb(argb) {}
    LAMBDA_FUNCTOR_EXPRESSION(select(arga, std::forward<Args>(args)...)[select(argb, std::forward<Args>(args)...)])
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
struct lambda_ref_functor {
    Arg &arg;
    lambda_ref_functor(Arg &arg): arg(arg) {}
    LAMBDA_FUNCTOR_EXPRESSION(select(arg, std::forward<Args>(args)...))
};
template<class A>
inline lambda_functor<lambda_ref_functor<A> > ref(A &a) { return lambda_ref_functor<A>(a); }
template<class A>
inline lambda_functor<lambda_ref_functor<const A> > ref(const A &a) { return lambda_ref_functor<const A>(a); }

#endif //LAMBDA_LAMBDA2_H
