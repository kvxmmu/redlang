//
// Created by kvxmmu on 17.04.2020.
//

#include "semantic.hpp"

namespace Names {
    const static std::string assign_variable = "assign_variable";
    const static std::string define_variable = "define_variable";
    const static std::string define_function = "define_function";
    const static std::string create_lambda = "create_lambda";

    const static std::string ordered_arg = "ordered_arg";
    const static std::string default_arg = "default_arg";
    const static std::string array_arg = "array_arg";
    const static std::string comma = "comma";
}

namespace SemanticSpace {
    static Pattern assignment = Pattern().add(ID).add(ASSIGNMENT).add("ANY").until_pattern(SEMICOLON);
    static Pattern define_variable = Pattern().add(ID).add(ID).add(ASSIGNMENT).add("ANY").until_pattern(SEMICOLON);
    static Pattern define_function = Pattern().add(FUNC).add(ID).add("$BRACKET").add(RIGHT_ARROW).add(ID).add("$BRACE");
    static Pattern create_lambda = Pattern().add("$BRACKET").add(RIGHT_ARROW).add("$BRACE");

    // function argument patterns
    static Pattern ordered_arg = Pattern().add(ID).add(ID);
    static Pattern default_arg = Pattern().add(ID).add(ID).add(ASSIGNMENT).add("_"+std::to_string(COMMA));
    static Pattern array_arg = Pattern().add(ID).add(ID).add("$SBRACKET");
    static Pattern comma = Pattern().add(COMMA);


    Semantizer top_level_semantizer = Semantizer()
            .add(Names::assign_variable,
                assignment)
            .add(Names::define_variable,
                 define_variable)
            .add(Names::define_function,
                 define_function)
            .add(Names::create_lambda,
                 create_lambda);

    Semantizer func_level_semantizer = Semantizer()
            .add(Names::assign_variable,
                    assignment)
            .add(Names::define_variable, define_variable)
            .add(Names::define_function, define_function)
            .add(Names::create_lambda, create_lambda);

    Semantizer arg_level_semantizer = Semantizer()
            .add(Names::default_arg, default_arg)
            .add(Names::array_arg, array_arg)
            .add(Names::ordered_arg, ordered_arg)
            .add(Names::comma, comma);

}


void run_semantic(AstTree &etree, RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> *common_iterator) {
    RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> *_it = nullptr;
    if (common_iterator != nullptr) {
        _it = common_iterator;
    } else {
        RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> i(etree.tree, nullptr);
        _it = &i;
    }
    RedIterator<std::vector<SimpleVariant*>, SimpleVariant*> &it = *_it;
    while (!it.is_done()) {
        SemantizerResponse res = SemanticSpace::top_level_semantizer.check(it);
        if (res.result == 0) {
            std::cerr << "Found piece of crap" << std::endl;
            exit(1);
        }
        if (res.pattern->name == Names::define_function)
            define_function(callback_auto);
        else if (res.pattern->name == Names::define_variable)
            define_variable(callback_auto);
        it.next(res.result);
    }
}

void define_function(callback_params) {
    std::string name = reinterpret_cast<Token*>(it.peek(1)->memptr)->token;
    SimpleVariant *args = it.peek(2);
    auto arguments = parse_this<SemTypes::FunctionArgument, token_type_t, 1, parse_args>(args, SemanticSpace::arg_level_semantizer);
    std::string return_type = reinterpret_cast<Token*>(it.peek(4)->memptr)->token;
    auto block = reinterpret_cast<AstTree*>(it.peek(5)->memptr);
    SemTypes::FunctionDefine function(name, return_type);
    std::cout << "Define function " << name << " with return type " << return_type << std::endl;
    std::cout << "Arguments: " << std::endl;
    for (SemTypes::FunctionArgument &argument : arguments) {
        std::cout << "\t" << argument.name << " with type " << argument.type;
        if (argument.is_array)
            std::cout << "[]";
        if (argument.default_value != nullptr)
            std::cout << " with default value";
        std::cout << std::endl;
    }
}

void define_variable(callback_params) {

}

void call_sea_man() {
    std::cout << "Блять, да мне похуй на тебя, блять, слушай, какая у тебя там тачка,\n"
                 "блять, квартиры, срачки там блять, яхты, всё, мне похуй, хоть там\n"
                 "\"Бэнтли\", хоть блять нахуй \"Майбах\", хоть \"Роллс-Ройс\", хоть \"Бугатти\"\n"
                 "блять, хоть стометровая яхта, мне на это насрать, понимаешь? Сколько ты\n"
                 "там, кого ебешь, каких баб, каких значит вот этих самок шикарных или\n"
                 "атласных, блять в космос ты летишь, мне на это насрать, понимаешь? Я,\n"
                 "блять, в своем познании настолько преисполнился, что я как будто бы уже\n"
                 "сто триллионов миллиардов лет, блять, проживаю на триллионах и\n"
                 "триллионах таких же планет, как эта Земля, мне этот мир абсолютно\n"
                 "понятен, и я здесь ищу только одного, блять, - покоя, умиротворения и\n"
                 "вот этой гармонии, от слияния с бесконечно вечным, от созерцания\n"
                 "великого фрактального подобия и от вот этого замечательного всеединства\n"
                 "существа, бесконечно вечного, куда ни посмотри, хоть вглубь - бесконечно\n"
                 " малое, хоть ввысь - бесконечное большое, понимаешь? А ты мне опять со\n"
                 "своим вот этим блять, иди суетись дальше, это твоё распределение, это\n"
                 "твой путь и твой горизонт познания и ощущения твоей природы, он\n"
                 "несоизмеримо мелок по сравнению с моим, понимаешь? Я как будто бы уже\n"
                 "давно глубокий старец, бессмертный, ну или там уже почти бессмертный,\n"
                 "который на этой планете от её самого зарождения, ещё когда только Солнце\n"
                 " только-только сформировалось как звезда, и вот это газопылевое облако,\n"
                 "вот, после взрыва, Солнца, когда оно вспыхнуло, как звезда, начало\n"
                 "формировать вот эти коацерваты, планеты, понимаешь, я на этой Земле уже\n"
                 "как будто почти пять миллиардов лет блять живу и знаю её вдоль и поперёк\n"
                 " этот весь мир, а ты мне какие-то... мне похуй на твои тачки, на твои\n"
                 "блять нахуй яхты, на твои квартиры, там, на твоё благо. Я был на этой\n"
                 "планете бесконечным множеством, и круче Цезаря, и круче Гитлера, и круче\n"
                 " всех великих, понимаешь, был, а где-то был конченым говном, ещё хуже,\n"
                 "чем здесь. Я множество этих состояний чувствую. Где-то я был больше\n"
                 "подобен растению, где-то я больше был подобен птице, там, червю, где-то\n"
                 "был просто сгусток камня, это всё есть душа, понимаешь? Она имеет грани\n"
                 "подобия совершенно многообразные, бесконечное множество. Но тебе этого\n"
                 "не понять, поэтому ты езжай себе блять, мы в этом мире как бы живем\n"
                 "разными ощущениями и разными стремлениями, соответственно, разное наше и\n"
                 " место, разное и наше распределение. Тебе я желаю все самые крутые тачки\n"
                 " чтоб были у тебя, и все самые лучше самки чтобы раздвигали ноги перед\n"
                 "тобой, чтобы раздвигали перед тобой щели, на шиворот-навыворот, блять,\n"
                 "перед тобой, как ковёр, это самое, раскрывали, растлевали, растлали, и\n"
                 "ты их чтобы ебал до посинения, докрасна, вон, как Солнце закатное, и\n"
                 "чтоб на лучших яхтах, и на самолётах летал, и кончал прямо с\n"
                 "иллюминатора, и делал всё, что только в голову могло прийти и не прийти,\n"
                 " если мало идей, обращайся ко мне, я тебе на каждую твою идею предложу\n"
                 "сотню триллионов, как всё делать. Ну а я всё, я иду как глубокий старец,\n"
                 " узревший вечное, прикоснувшийся к Божественному, сам стал богоподобен и\n"
                 " устремлен в это бесконечное, и который в умиротворении, покое,\n"
                 "гармонии, благодати, в этом сокровенном блаженстве пребывает,\n"
                 "вовлеченный во всё и во вся, понимаешь, вот и всё, в этом наша разница.\n"
                 "Так что я иду любоваться мирозданием, а ты идёшь преисполняться в ГРАНЯХ\n"
                 " каких-то, вот и вся разница, понимаешь, ты не зришь это вечное\n"
                 "бесконечное, оно тебе не нужно. Ну зато ты, так сказать, более активен,\n"
                 "как вот этот дятел долбящий, или муравей, который очень активен в своей\n"
                 "стезе, поэтому давай, наши пути здесь, конечно, имеют грани подобия,\n"
                 "потому что всё едино, но я-то тебя прекрасно понимаю, а вот ты меня -\n"
                 "вряд ли, потому что я как бы тебя в себе содержу, всю твою природу, она\n"
                 "составляет одну маленькую там песчиночку, от того что есть во мне, вот и\n"
                 " всё, поэтому давай, ступай, езжай, а я пошел наслаждаться нахуй блять\n"
                 "прекрасным осенним закатом на берегу теплой южной реки. Всё,\n"
                 "пиздуй-бороздуй, и я попиздил, нахуй" << std::endl;
    exit(0);
}

void parse_args(SemantizerResponse &res, std::vector<SemTypes::FunctionArgument> &args,
        DefaultIt &it, token_type_t &last) {
    std::string name = res.pattern->name;
    if (name == Names::comma) {
        if (last == COMMA)
            call_sea_man();
        last = COMMA;
        return;
    }
    if (last != COMMA && last != 1)
        call_sea_man();
    std::string type = reinterpret_cast<Token*>(it.peek(0)->memptr)->token;
    std::string id = reinterpret_cast<Token*>(it.peek(1)->memptr)->token;
    if (name == Names::ordered_arg || name == Names::array_arg) {
        SemTypes::FunctionArgument arg(type, id, name == Names::array_arg);
        args.push_back(arg);
    } else if (name == Names::default_arg) {
        SimpleVariant *default_value = it.peek(3);
        SemTypes::FunctionArgument &arg = SemTypes::FunctionArgument(type, id, false).set_default_value(default_value);
        args.push_back(arg);
    } else {
        call_sea_man();
    }
    last = 0;
}

token_type_t convert(const std::string &ref) {
    if (ref == "BRACKET")
        return BRACKET;
    else if (ref == "SBRACKET")
        return SBRACKET;
    else if (ref == "ANY")
        return NOTHING;
    return BRACE;
}


size_t Pattern::match(RedIterator<std::vector <SimpleVariant *>, SimpleVariant *>& it) {
    if (this->custom_match != nullptr) {
        return this->custom_match(it);
    }
    size_t pos = 0;
    while (!it.is_done(pos) && (pos < this->pattern.size())) {
        SimpleVariant *variant = it.peek(pos);
        std::string &var = this->pattern[pos];
        if (!this->check_pattern(var, variant))
            return 0;
        pos++;
    }
    if (!this->get_until.empty()) {
        while (!it.is_done(pos)) {
            SimpleVariant *variant = it.peek(pos);
            if (this->check_pattern(this->get_until, variant))
                return pos+1;
            pos++;
        }
    }
    if (pos < this->pattern.size())
        return 0;
    return pos;
}

bool Pattern::check_pattern(std::string str, SimpleVariant *variant) {
    if (str == "ANY")
        return true;
    bool by_text = str.at(0) == '!';
    bool by_block = str.at(0) == '$';
    bool not_ = str.at(0) == '_';

    if (by_text || by_block || not_) {
        str = str.substr(1);
    }
    if (by_text && variant->type == SIMPLE_OBJECT) {
        auto pToken = reinterpret_cast<Token*>(variant->memptr);
        return pToken->token == str;
    } else if (by_block && variant->type == TREE) {
        token_type_t tree_type = convert(str);
        if (tree_type == NOTHING)
            return true;
        auto pTree = reinterpret_cast<AstTree*>(variant->memptr);
        return tree_type == pTree->block_type;
    } else if (variant->type == SIMPLE_OBJECT && !by_text && !by_block) {
        auto pToken = reinterpret_cast<Token*>(variant->memptr);
        bool ret = pToken->type == std::stoi(str);
        return not_ == !ret;
    }
    return false;
}
