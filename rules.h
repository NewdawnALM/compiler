#ifndef  __RULES_H
#define  __RULES_H

// 各种文法词法的规则

namespace myGrammar85 {           // p85
    Symbol P('P', Symbol::startSign), L('L'), S('S'), L1('H'), sem(';'), NUL, a('a'), V('V'), c('c'), i('i'), V1('I'), com(','),
            E('E'), T('T'), E1('J'), plus('+'), sub('-'), F('F'), T1('K'), mul('*'), div('/'), lp('('), rp(')'),
            x('x'), y('y'), eq('='), lb('{'), rb('}');
    typedef std::vector<Symbol> vsym;
    std::vector<Production> pro{ {P, vsym{lb, L, rb}}, {L, vsym{S, L1}}, {L1, vsym{sem, S, L1}}, {L1, vsym{NUL}},
        {S, vsym{a, V}}, {S, vsym{c, V}}, {V, vsym{i, V1}}, {V1, vsym{com, i, V1}}, {V1, vsym{NUL}}, {S, vsym{i, eq, E}},
        {E, vsym{T, E1}}, {E1, vsym{plus, T, E1}}, {E1, vsym{sub, T, E1}}, {E1, vsym{NUL}}, {T, vsym{F, T1}},
        {T1, vsym{mul, F, T1}}, {T1, vsym{div, F, T1}}, {T1, vsym{NUL}}, {F, vsym{lp, E, rp}}, {F, vsym{sub, F}},
        {F, vsym{plus, F}}, {F, vsym{i}}, {F, vsym{x}}, {F, vsym{y}}
    };
}

namespace myGrammar82 {          // p82
    Symbol S('S', Symbol::startSign), S1('T'), C('C'), f('f'), t('t'), a('a'), e('e'), i('i'), NUL;
    typedef std::vector<Symbol> vsym;
    std::vector<Production> pro{ {S, vsym{f, C, t, S, S1}}, {S, vsym{a}}, {S1, vsym{e, S}}, {S1, vsym{NUL}}, {C, vsym{i}} };
}

namespace myGrammar76 {          // p76
    Symbol E('E', Symbol::startSign), T('T'), E1('H'), T1('I'), F('F'), plus('+'), mul('*'),
            lp('('), rp(')'), i('i'), x('x'), y('y'), NUL;
    typedef std::vector<Symbol> vsym;
    std::vector<Production> pro{ {E, vsym{T, E1}}, {E1, vsym{plus, T, E1}}, {E1, vsym{NUL}}, {T, vsym{F, T1}},
        {T1, vsym{mul, F, T1}}, {T1, vsym{NUL}}, {F, vsym{lp, E, rp}}, {F, vsym{i}}, {F, vsym{x}}, {F, vsym{y}}
    };
}

namespace myGrammar89 {          // p89
    Symbol S('S', Symbol::startSign), a('a'), pow('^'), lp('('), rp(')'), T('T'), T1('H'), com(','), NUL;
    typedef std::vector<Symbol> vsym;
    std::vector<Production> pro{ {S, vsym{a}}, {S, vsym{pow}}, {S, vsym{lp, T, rp}}, {T, vsym{S, T1}},
        {T1, vsym{com, S, T1}}, {T1, vsym{NUL}},
    };
}

namespace myGrammar92 {          // p92
    Symbol S('S', Symbol::startSign), S1('T'), C('C'), f('f'), t('t'), a('a'), e('e'), j('j'), i('i');
    typedef std::vector<Symbol> vsym;
    std::vector<Production> pro { {S, vsym{f, C, t, S, S1}}, {S, vsym{a}}, {S1, vsym{e, S}}, {S1, vsym{j}}, {C, vsym{i}} };
}


namespace myNFA25 {             // p25
    Status X("X"), _1("1"), _2("2"), _3("3"), _4("4"), Y("Y", true);
    bool sign = false;
    const Status& start() {
        if(sign == true)    return X;
        sign = true;
        X.next[Symbol::NULCHAR].insert(&_1);
        _1.next[Symbol::NULCHAR].insert(&_2);
        _1.next['a'].insert(&_1);     _1.next['b'].insert(&_1);
        _2.next['a'].insert(&_3);     _2.next['b'].insert(&_4);
        _3.next['a'].insert(&Y);
        _4.next['b'].insert(&Y);
        return X;
    }
}

namespace myNFA28 {             // p28
    Status X("X"), _1("1"), _2("2"), Y("Y", true);
    bool sign = false;
    const Status& start() {
        if(sign == true)    return X;
        sign = true;
        X.next['a'].insert(&_1);      X.next['b'].insert(&_1);
        _1.next[Symbol::NULCHAR].insert(&_2);
        _2.next['a'].insert(&_2);     _2.next['b'].insert(&_2);
        _2.next['0'].insert(&_2);     _2.next['1'].insert(&_2);
        _2.next[Symbol::NULCHAR].insert(&Y);
        return X;
    }
}

namespace myNFA41_2_14 {          // p41
    Status X("X"), _1("1"), _2("2"), _3("3"), _4("4"), _5("5"), Y("Y", true);
    bool sign = false;
    const Status& start() {
        if(sign == true)    return X;
        sign = true;
        X.next['1'].insert(&_1);          _1.next[Symbol::NULCHAR].insert(&_2);
        _2.next['0'].insert(&_2);         _2.next['1'].insert(&_2);
        _2.next[Symbol::NULCHAR].insert(&_3);     _3.next['1'].insert(&_4);
        _4.next['0'].insert(&_5);         _5.next['1'].insert(&Y);
        return X;
    }
}

namespace myNFA41_2_16 {          // p41
    Status X("X"), _1("1"), _2("2"), Y("Y", true);
    bool sign = false;
    const Status& start() {
        if(sign == true)    return X;
        sign = true;
        X.next[Symbol::NULCHAR].insert(&_1);
        _1.next['0'].insert(&_1);         _1.next['1'].insert(&_2);
        _2.next['0'].insert(&_1);         _1.next[Symbol::NULCHAR].insert(&Y);
        return X;
    }
}

namespace myNFA42 {             // p42
    Status _0("0"), _1("1", true), _2("2"), _3("3"), _4("4"),
            _5("5", true), _6("6", true), _7("7"), _8("8", true);
    bool sign = false;
    const Status& start() {
        if(sign == true)    return _0;
        sign = true;
        _0.next['<'].insert(&_1);        _0.next['<'].insert(&_2);
        _0.next['<'].insert(&_3);         _0.next['1'].insert(&_4);
        _0.next['0'].insert(&_4);         _2.next['<'].insert(&_5);
        _3.next['='].insert(&_6);         _4.next[Symbol::NULCHAR].insert(&_7);
        _7.next['0'].insert(&_7);         _7.next['1'].insert(&_7);
        _7.next[Symbol::NULCHAR].insert(&_8);
        return _0;
    }
}

namespace myNFA30 {             // p30
    Status _0("0"), _1("1"), _2("2"), _3("3", true), _4("4"),
            _5("5", true), _6("6", true), _7("7", true), _8("8", true),
            _9("9", true), _10("10", true), _11("11", true), _12("12"),
            _13("13", true), _14("14"), _15("15", true), _16("16", true);
    bool sign = false;
    const Status& start() {
        if(sign == true)    return _0;
        sign = true;
        static const char Alpha = 'a', Beta = '0';
        _0.next[Alpha].insert(&_1);        _0.next[Beta].insert(&_2);
        _0.next['='].insert(&_3);        _0.next['+'].insert(&_4);
        _0.next['+'].insert(&_5);        _0.next['*'].insert(&_6);
        _0.next[','].insert(&_7);        _0.next[';'].insert(&_8);
        _0.next['('].insert(&_9);        _0.next[')'].insert(&_10);
        _0.next['#'].insert(&_11);            _1.next[Symbol::NULCHAR].insert(&_12);
        _2.next[Symbol::NULCHAR].insert(&_14);        _4.next['+'].insert(&_16);
        _12.next[Alpha].insert(&_12);           _12.next[Beta].insert(&_12);
        _12.next[Symbol::NULCHAR].insert(&_13);       _14.next[Beta].insert(&_14);
        _14.next[Symbol::NULCHAR].insert(&_15);
        return _0;
    }
}

namespace myNFA {             // 我自己的，在 myNFA30 的基础上增添了浮点数的识别
    Status _0("0"), _1("1"), _2("2"), _3("3", true), _4("4", true),
            _5("5", true), _6("6", true), _7("7", true), _8("8", true),
            _9("9", true), _10("10", true), _11("11", true), _12("12", true),
            _13("13"), _14("14", true), _15("15"), _16("16", true),
            _17("17"), _18("18"), _19("19", true), _20("20"),
            _21("21"), _22("22"), _23("23", true), _24("24"),
            _25("25"), _26("26"), _27("27"), _28("28"),
            _29("29", true);
    static const char letter_except_Ee = 'a', number = '0', Ee = 'e';

    bool sign = false;
    const Status& start() {
        if(sign == true)    return _0;
        sign = true;

        _0.next[Ee].insert(&_1);            _0.next[letter_except_Ee].insert(&_1);
        _0.next[number].insert(&_2);
        _0.next['='].insert(&_3);           _0.next['+'].insert(&_4);
        _0.next['-'].insert(&_5);           _0.next['*'].insert(&_6);
        _0.next['/'].insert(&_7);           _0.next[','].insert(&_8);
        _0.next[';'].insert(&_9);           _0.next['('].insert(&_10);
        _0.next[')'].insert(&_11);          _0.next['#'].insert(&_12);
        _0.next['.'].insert(&_20);

        _1.next[Symbol::NULCHAR].insert(&_13);
        _13.next[Ee].insert(&_13);          _13.next[letter_except_Ee].insert(&_13);
        _13.next[number].insert(&_13);      _13.next[Symbol::NULCHAR].insert(&_14);

        _2.next[Symbol::NULCHAR].insert(&_15);
        _15.next[number].insert(&_15);      _15.next[Symbol::NULCHAR].insert(&_16);
        _16.next['.'].insert(&_17);         _16.next[Ee].insert(&_24);
        _17.next[Symbol::NULCHAR].insert(&_18);
        _18.next[number].insert(&_18);      _18.next[Symbol::NULCHAR].insert(&_19);
        _19.next[Ee].insert(&_24);

        _20.next[number].insert(&_21);
        _21.next[Symbol::NULCHAR].insert(&_22);
        _22.next[number].insert(&_22);       _22.next[Symbol::NULCHAR].insert(&_23);
        _23.next[Ee].insert(&_24);
        _24.next['+'].insert(&_26);         _24.next['-'].insert(&_26);
        _24.next[Symbol::NULCHAR].insert(&_25);
        _25.next[Symbol::NULCHAR].insert(&_26);
        _26.next[number].insert(&_27);
        _27.next[Symbol::NULCHAR].insert(&_28);
        _28.next[number].insert(&_28);      _28.next[Symbol::NULCHAR].insert(&_29);

        return _0;
    }
}

#endif // __RULES_H
