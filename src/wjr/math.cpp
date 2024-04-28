#include <wjr/math.hpp>

namespace wjr {

// compare.hpp
template int large_builtin_compare_n<uint64_t>(const uint64_t *src0, const uint64_t *src1,
                                               size_t n);

template int large_builtin_reverse_compare_n<uint64_t>(const uint64_t *src0,
                                                       const uint64_t *src1, size_t n);

// find.hpp
template size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src0,
                                                   const uint64_t *src1, size_t n);

template size_t large_builtin_find_not_n<uint64_t>(const uint64_t *src, uint64_t val,
                                                   size_t n);

template size_t large_builtin_reverse_find_not_n<uint64_t>(const uint64_t *src0,
                                                           const uint64_t *src1,
                                                           size_t n);

template size_t large_builtin_reverse_find_not_n<uint64_t>(const uint64_t *src,
                                                           uint64_t val, size_t n);

// mul.hpp

template void toom22_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                     size_t n, const uint64_t *src1, size_t m,
                                     uint64_t *stk);

template void toom2_sqr<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                  size_t n, uint64_t *stk);

template void toom32_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                     size_t n, const uint64_t *src1, size_t m,
                                     uint64_t *stk);

template void
toom_interpolation_5p_s<uint64_t>(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                                  size_t rn, size_t rm,
                                  toom_interpolation_5p_struct<uint64_t> &&flag);

template void toom42_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                     size_t n, const uint64_t *src1, size_t m,
                                     uint64_t *stk);

template void toom33_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                     size_t n, const uint64_t *src1, size_t m,
                                     uint64_t *stk);

template void toom3_sqr<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                  size_t n, uint64_t *stk);

template void
toom_interpolation_6p_s<uint64_t>(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                                  size_t rn, size_t rm,
                                  toom_interpolation_6p_struct<uint64_t> &&flag);

template void toom43_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                     size_t n, const uint64_t *src1, size_t m,
                                     uint64_t *stk);

template void
toom_interpolation_7p_s<uint64_t>(uint64_t *WJR_RESTRICT dst, uint64_t *w1p, size_t l,
                                  size_t rn, size_t rm,
                                  toom_interpolation_7p_struct<uint64_t> &&flag);

template void toom53_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                     size_t n, const uint64_t *src1, size_t m,
                                     uint64_t *stk);

template void toom44_mul_s<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src0,
                                     size_t n, const uint64_t *src1, size_t m,
                                     uint64_t *stk);

template void toom4_sqr<uint64_t>(uint64_t *WJR_RESTRICT dst, const uint64_t *src,
                                  size_t n, uint64_t *stk);

template void __noinline_mul_s_impl<true, uint64_t>(uint64_t *WJR_RESTRICT dst,
                                                    const uint64_t *src0, size_t n,
                                                    const uint64_t *src1, size_t m,
                                                    uint64_t *mal);

template void __noinline_mul_s_impl<false, uint64_t>(uint64_t *WJR_RESTRICT dst,
                                                     const uint64_t *src0, size_t n,
                                                     const uint64_t *src1, size_t m,
                                                     in_place_empty_t mal);

// div.hpp

template uint64_t sb_div_qr_s<uint64_t>(uint64_t *dst, uint64_t *src, size_t n,
                                        const uint64_t *div, size_t m, uint64_t dinv);

template uint64_t dc_div_qr_s<uint64_t>(uint64_t *dst, uint64_t *src, size_t n,
                                        const uint64_t *div, size_t m, uint64_t dinv);

template void div_qr_s<uint64_t>(uint64_t *dst, uint64_t *rem, const uint64_t *src,
                                 size_t n, const uint64_t *div, size_t m);

// convert.hpp

const precompute_chars_convert_16n_t precompute_chars_convert_16n[36 - 6] = {
    {12157665459056928801ull,
     16ull,
     40,
     640,
     {12804155896070955521ull, 15543579786812514016ull, 13845876942000183002ull,
      2400930438297476448ull, 4252349766273249422ull, 3225869540951928930ull,
      17531300544910775922ull, 11984534816691816635ull, 962477687161620039ull,
      1216612531246616941ull, 9408595874794799783ull, 6860772283536875451ull,
      2838538504260284862ull, 1252419825538020332ull, 12540383894939280105ull,
      23377990787817727ull}},
    {7450580596923828125ull,
     16ull,
     27,
     432,
     {9919726386361907521ull, 4630289419283684731ull, 11654214312702319081ull,
      10984660978606903562ull, 16424365659841183319ull, 13590481813851882819ull,
      5543310244362305101ull, 3883019953012795806ull, 533459625314589732ull,
      15086276895428384221ull, 17935562173010865467ull, 2675836009788787840ull,
      4701091480357897553ull, 12350468064450722389ull, 14299717677152412631ull,
      9252221807202ull}},
    {4738381338321616896ull,
     10ull,
     24,
     384,
     {12026854307803393537ull, 6587327226042281620ull, 5949193692463572742ull,
      16340864791353723037ull, 4895750586451757400ull, 4993145721471861013ull,
      10351807525191341928ull, 17983159292351844974ull, 12052984924518600484ull,
      6626501641ull}},
    {3909821048582988049ull,
     16ull,
     22,
     352,
     {7707442872082794753ull, 10938623941787649248ull, 11272607700166710193ull,
      11342399378302396797ull, 3027882661123691895ull, 2198439938856963296ull,
      2100224287646679737ull, 391321536443572872ull, 5239919838627929044ull,
      3068599383993525461ull, 17204283608533776146ull, 18123848486126451005ull,
      13921477660434433763ull, 2217127029335112124ull, 15932117664541013016ull,
      305994941ull}},
    {12157665459056928801ull,
     16ull,
     20,
     320,
     {12804155896070955521ull, 15543579786812514016ull, 13845876942000183002ull,
      2400930438297476448ull, 4252349766273249422ull, 3225869540951928930ull,
      17531300544910775922ull, 11984534816691816635ull, 962477687161620039ull,
      1216612531246616941ull, 9408595874794799783ull, 6860772283536875451ull,
      2838538504260284862ull, 1252419825538020332ull, 12540383894939280105ull,
      23377990787817727ull}},
    {10000000000000000000ull,
     12ull,
     19,
     304,
     {18104751977006104576ull, 7022382078611961980ull, 5818713298520399752ull,
      3845577022746030748ull, 12139943004249278607ull, 5889242339408380438ull,
      14552017514495643209ull, 4968216578135424189ull, 15773629750718909556ull,
      9342075265934726546ull, 1149859480163044520ull, 1026134200324594ull}},
    {5559917313492231481ull,
     16ull,
     18,
     288,
     {17215257851429187969ull, 14770332620004888372ull, 11371989238901517101ull,
      7780909903494194240ull, 8353828368129419470ull, 4289818026421302562ull,
      6997211172397184407ull, 16553752467361937302ull, 6251993913714420438ull,
      9919300797542775483ull, 2112142636816701748ull, 10801774822524604312ull,
      16842279848885594002ull, 4804208755920687956ull, 14977752411443706250ull,
      85565262719ull}},
    {2218611106740436992ull,
     8ull,
     17,
     272,
     {3668968326480003072ull, 2856077273845143404ull, 429166167341648605ull,
      5621345344265602246ull, 16949049212610825448ull, 4935989525122558810ull,
      4676044287813493157ull, 35359ull}},
    {8650415919381337933ull,
     16ull,
     17,
     272,
     {9538382564635240513ull, 2460466646521485275ull, 16666209249482607825ull,
      14809018514960478104ull, 3655832742951236681ull, 18037631742675398617ull,
      16931896012088359140ull, 13553328343921229598ull, 6198197414158418225ull,
      16910829582624025791ull, 684569413823022046ull, 9635521441798820106ull,
      4834948745726145906ull, 17369808863745746958ull, 6778168315101934796ull,
      100877890056870ull}},
    {2177953337809371136ull,
     12ull,
     16,
     256,
     {582341683539466241ull, 935252957464965720ull, 7598000487062652093ull,
      4625010431675930147ull, 9224048018587399532ull, 13680868751916767356ull,
      8922574898880355762ull, 10713662064272255971ull, 7311846544833571521ull,
      7849590305058879299ull, 10093391657513723873ull, 26301ull}},
    {6568408355712890625ull,
     16ull,
     16,
     256,
     {56793839378067457ull, 10809444194805465837ull, 4861426308971612881ull,
      12907708596461028372ull, 6172393804453828068ull, 17672938504419320786ull,
      17934876766114597666ull, 141829255137314267ull, 6773855699174813111ull,
      10028193115799165882ull, 9856349050108160170ull, 15401037017628404120ull,
      13762734768330464360ull, 5487256294993204681ull, 12930548835409038001ull,
      1231874543916ull}},
    {2862423051509815793ull,
     16ull,
     15,
     240,
     {11865239832077866753ull, 17986596523288397197ull, 15432472163376587275ull,
      5879474424838572282ull, 17136338192325314705ull, 7535862621637422047ull,
      9480299764456790599ull, 16250052460691387039ull, 7837061892006431614ull,
      2190291071455739606ull, 2089763779773940451ull, 7449376331858278405ull,
      13526866592217535857ull, 9828582586444045776ull, 5962505148965397652ull,
      2084228ull}},
    {6746640616477458432ull,
     13ull,
     15,
     240,
     {16825729682832883712ull, 12736036210993412177ull, 17454163961828053367ull,
      1566810466835755517ull, 10408966924619397049ull, 5962762537783533235ull,
      7027539632117998796ull, 233664546091283778ull, 6581133406537352961ull,
      1731286874565754319ull, 16692151976336513598ull, 11481756170242148033ull,
      1890624960989ull}},
    {15181127029874798299ull,
     16ull,
     15,
     240,
     {1523301832892496321ull, 15741395637969734571ull, 8987021470193812626ull,
      11729726669049843182ull, 2464431824755225162ull, 14809849291060860318ull,
      7917125097357611903ull, 14059678935630607960ull, 14193243186042062773ull,
      8911989858070342383ull, 675572746642225010ull, 5724872752466340158ull,
      16547776643085869396ull, 6571153659299016676ull, 17603075107400646255ull,
      816710974172328047ull}},
    {1638400000000000000ull,
     9ull,
     14,
     224,
     {4777629371499269249ull, 18001405160525099782ull, 4879812164015670080ull,
      14509320244552862897ull, 2636856509737464972ull, 2417179124242652079ull,
      8851510277453438840ull, 11902450618118332165ull, 276ull}},
    {3243919932521508681ull,
     16ull,
     14,
     224,
     {1377932995175387777ull, 16840020422497535136ull, 18409313831725898541ull,
      3549557770372876118ull, 479369023932380059ull, 9373360031190647754ull,
      12444814877489239138ull, 672316205119464860ull, 14469063119197155922ull,
      2962037997159928987ull, 2635214843369238475ull, 3066385631087808916ull,
      14671195167986263673ull, 14723683851189462789ull, 680944512491956658ull,
      15428588ull}},
    {6221821273427820544ull,
     13ull,
     14,
     224,
     {10774763706488193024ull, 14922209563121946000ull, 12187385192774810759ull,
      18329717913940786150ull, 16175211087733918847ull, 10680978913329253720ull,
      10222708645245701941ull, 8755715633419261510ull, 15712262099707476650ull,
      6381547515496574155ull, 4159227993444787714ull, 16023207754606163881ull,
      517469392140ull}},
    {11592836324538749809ull,
     16ull,
     14,
     224,
     {183687441944948481ull, 1364508147649575844ull, 7638530248297899994ull,
      2277824937317657793ull, 13540431965554716540ull, 2843503458107980791ull,
      1923200913990864062ull, 1868668140917812654ull, 15023155623599989137ull,
      4955057518620469417ull, 1214830557559987013ull, 1449672974335699914ull,
      4875114486981661208ull, 3891698203702506020ull, 13583099457371212384ull,
      10920422233834974ull}},
    {876488338465357824ull,
     7ull,
     13,
     221,
     {0ull, 13927585846938042368ull, 17205651709300675357ull, 6668724242937146186ull,
      9851154169731921908ull, 13361496707034599163ull, 10911603610649792ull}},
    {1490116119384765625ull,
     16ull,
     13,
     208,
     {15820528498683652481ull, 12918883675045082715ull, 12701045365820479403ull,
      9411582556959981489ull, 5207675975859366162ull, 14464666515629495023ull,
      6610471113138561902ull, 1254846493944523272ull, 2176561734499639137ull,
      10817343951842396781ull, 12698738035792448321ull, 10456972557441313090ull,
      15285296467118226778ull, 8742573488934804642ull, 11720338730322957869ull, 60ull}},
    {2481152873203736576ull,
     13ull,
     13,
     208,
     {2561882385218076672ull, 12485278750443544988ull, 5416385285399001613ull,
      7678641240427221077ull, 2128257956566851694ull, 6607128621106773669ull,
      16583864210029036124ull, 12043282020979838568ull, 15002248862215793845ull,
      17045058614492059605ull, 4485671480559885403ull, 11064959987178238393ull,
      211670ull}},
    {4052555153018976267ull,
     16ull,
     13,
     208,
     {10156215962519050945ull, 1939409935729187690ull, 12244721317850271851ull,
      3822387580760467750ull, 11552142005466058455ull, 8747662050942688732ull,
      6121721969463705703ull, 675220537997421297ull, 10834751043009527439ull,
      14863326317009207943ull, 109616935728104137ull, 3377992553321772101ull,
      6476782721482580073ull, 6880920997238627091ull, 9624887403949681380ull,
      543084124ull}},
    {6502111422497947648ull,
     10ull,
     13,
     208,
     {9038407247322939392ull, 7494194493048989796ull, 14413700761426029845ull,
      10338462119099779375ull, 13171982047895135425ull, 2107363432800569199ull,
      10330183245413639645ull, 9210977687586946502ull, 12330655122399442180ull,
      1047308628917ull}},
    {10260628712958602189ull,
     16ull,
     13,
     208,
     {12197004318425698369ull, 6575332209889136201ull, 3076111774013529400ull,
      7647939681601352051ull, 3705126780808654849ull, 4063771237409894580ull,
      17855178775940592007ull, 719854213968894528ull, 13654482137354242021ull,
      16947249212567557418ull, 16023319591577725213ull, 7914115804303548741ull,
      15361901329930313548ull, 5289337480849841122ull, 11610738855200026864ull,
      1548772845419194ull}},
    {15943230000000000000ull,
     13ull,
     13,
     208,
     {2749155484853403648ull, 1001927545556509336ull, 4759945227294703648ull,
      18126369247763408608ull, 17995146214110743504ull, 14356630362038292642ull,
      6274605540554118318ull, 13064267535266523386ull, 13349731420990492079ull,
      7089323062805432053ull, 14087207210800128282ull, 4198853260137289296ull,
      1788237278938394420ull}},
    {787662783788549761ull,
     16ull,
     12,
     204,
     {8950058137388674689ull, 15344693465118017798ull, 3179881658697124738ull,
      11421174673190643188ull, 9827833596754838403ull, 13704200073421521792ull,
      14332985786414328762ull, 6603261747871220865ull, 10045608427777477891ull,
      17760561582159419553ull, 17837480963368209627ull, 9486896648162627922ull,
      10271608905314817631ull, 7165977372661389591ull, 8805882629021357759ull,
      1774147423084687ull}},
    {1667889514952984961ull,
     16ull,
     12,
     192,
     {4412087895116912641ull, 12954810258004737190ull, 2543791547967345868ull,
      17554208142285752996ull, 10261177784421782509ull, 9232032334510028027ull,
      5917426908056764944ull, 7913461335343934555ull, 9953327488372462659ull,
      1427201239198300203ull, 108328946489922394ull, 15236778693879608793ull,
      7091765020967187705ull, 2735431826464583677ull, 512719163973020407ull, 368ull}},
    {2386420683693101056ull,
     13ull,
     12,
     192,
     {2552899673221999617ull, 11182898011852586843ull, 6617694042083818412ull,
      1506425320672855210ull, 7177755319314235328ull, 2579769913130518010ull,
      11924310169688603701ull, 3622708545797922487ull, 4428963946901488822ull,
      1594660323606243311ull, 7627719818394370652ull, 16974845366170711107ull,
      113541ull}},
    {3379220508056640625ull,
     16ull,
     12,
     192,
     {4257974156441272065ull, 16615099177768987455ull, 6836515386969469340ull,
      10734585312454726413ull, 9450674993360875104ull, 10402389585737597860ull,
      16056260359548279884ull, 8182688441970484362ull, 14082685963822677691ull,
      5585047079456446442ull, 3895296109061153305ull, 12866889272013202074ull,
      13108300719712594278ull, 1531308829054445336ull, 11165543261341628675ull,
      29666661ull}},
    {4738381338321616896ull,
     10ull,
     12,
     192,
     {12026854307803393537ull, 6587327226042281620ull, 5949193692463572742ull,
      16340864791353723037ull, 4895750586451757400ull, 4993145721471861013ull,
      10351807525191341928ull, 17983159292351844974ull, 12052984924518600484ull,
      6626501641ull}}};

const std::array<const precompute_chars_convert_16n_t *, 37>
    precompute_chars_convert_16n_ptr = {nullptr,
                                        nullptr,
                                        nullptr,
                                        &precompute_chars_convert_16n[0],
                                        nullptr,
                                        &precompute_chars_convert_16n[1],
                                        &precompute_chars_convert_16n[2],
                                        &precompute_chars_convert_16n[3],
                                        nullptr,
                                        &precompute_chars_convert_16n[4],
                                        &precompute_chars_convert_16n[5],
                                        &precompute_chars_convert_16n[6],
                                        &precompute_chars_convert_16n[7],
                                        &precompute_chars_convert_16n[8],
                                        &precompute_chars_convert_16n[9],
                                        &precompute_chars_convert_16n[10],
                                        nullptr,
                                        &precompute_chars_convert_16n[11],
                                        &precompute_chars_convert_16n[12],
                                        &precompute_chars_convert_16n[13],
                                        &precompute_chars_convert_16n[14],
                                        &precompute_chars_convert_16n[15],
                                        &precompute_chars_convert_16n[16],
                                        &precompute_chars_convert_16n[17],
                                        &precompute_chars_convert_16n[18],
                                        &precompute_chars_convert_16n[19],
                                        &precompute_chars_convert_16n[20],
                                        &precompute_chars_convert_16n[21],
                                        &precompute_chars_convert_16n[22],
                                        &precompute_chars_convert_16n[23],
                                        &precompute_chars_convert_16n[24],
                                        &precompute_chars_convert_16n[25],
                                        nullptr,
                                        &precompute_chars_convert_16n[26],
                                        &precompute_chars_convert_16n[27],
                                        &precompute_chars_convert_16n[28],
                                        &precompute_chars_convert_16n[29]};

precompute_chars_convert_t *precompute_chars_convert(precompute_chars_convert_t *pre,
                                                     size_t n, unsigned int base,
                                                     uint64_t *table_mem) {
    const precompute_chars_convert_16n_t *p16n = precompute_chars_convert_16n_ptr[base];
    const uint64_t big_base = p16n->big_base;
    const size_t digits_in_one_base = p16n->digits_in_one_base;

    auto set = [base](precompute_chars_convert_t *pre, const uint64_t *ptr, size_t n,
                      size_t shift, size_t digits_in_base) {
        *pre = {ptr, n, shift, digits_in_base, base};
    };

    size_t digits = p16n->n;
    size_t shift = 16 - digits;
    size_t digits_in_base = p16n->digits_in_sixteen_base;

    set(pre, nullptr, 0, 0, 0);
    ++pre;
    set(pre, p16n->arr, digits, shift, digits_in_base);

    while (n * 2 > (digits + shift) * 5) {
        sqr(table_mem, pre->ptr, digits);
        digits <<= 1;
        shift <<= 1;
        digits_in_base <<= 1;

        int c = table_mem[0] == 0;
        table_mem += c;
        digits -= c;
        shift += c;

        if (WJR_UNLIKELY(table_mem[digits - 1] == 0)) {
            table_mem[digits - 1] = mul_1(table_mem, table_mem, digits - 1, big_base);
            digits_in_base += digits_in_one_base;

            c = table_mem[0] == 0;
            table_mem += c;
            digits -= c;
            shift += c;
        }

        ++pre;
        set(pre, table_mem, digits, shift, digits_in_base);
        table_mem += digits;
    }

    return pre;
}

} // namespace wjr