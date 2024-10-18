#include "crc64.h"



static const uint_least64_t crc64_table[]=
{
0, 12911341560706588527, 17619267392293085275, 5164075066763771700, 8921845837811637811, 14483170935171449180, 10328150133527543400, 4357999468653093127,
17843691675623275622, 4940391307328217865, 226782375002905661, 12685511915359257426, 10119945210068853333, 4566377562367245626, 8715998937306186254, 14689403211693301089,
9051005139383707209, 14895072503764629798, 9880782614656435730, 4193374422961527165, 453564750005811322, 13070904082541799189, 17496296445768931361, 4747102235666401102,
9960315520700766767, 4113029525020509504, 9132755124734491252, 14812441257301386523, 17431997874612372508, 4811156168024382323, 391483189436228679, 13132671735097031464,
18102010278767414418, 5195199925788447741, 1131375642422963401, 13591081480414639014, 9288535643022529185, 3731739485546663374, 8386748845923054330, 14361410892855143829,
907129500011622644, 13814943346342178715, 17875617253995106479, 5421418680781082560, 8594564625313771207, 14152643483341451688, 9494204471332802204, 3525329033817543155,
9704381199536204507, 3855837706121835956, 8226059050041019008, 13908973417437222383, 18265510249468982504, 5643692520190618503, 718348998302913715, 13463047253836762076,
8146277531524994749, 13989069943491807698, 9622312336048764646, 3938150108875254153, 782966378872457358, 13399312233903888353, 18327840216347633877, 5582173445676054458,
7257036000092981153, 15535280666427316430, 10390399851576895482, 2529986302517213333, 2262751284845926802, 12414353723947190013, 16997392145760156105, 6398650419759490726,
10599130201908394951, 2322133910755632296, 7463478971093326748, 15329644185724306675, 16773497691846108660, 6622864283287239323, 2036569382881248687, 12640783567252986560,
1814259000023245288, 12250853444207230599, 17125426475222188467, 6811676960462675676, 7132938157145702363, 15119434731753103540, 10842837361562165120, 2690676064372932847,
17189129250627542414, 6747026957542163169, 1875814858707893717, 12188560364711551674, 10762704257491731389, 2770420489343360210, 7050658067635086310, 15201536148867841161,
11493583972846619443, 3219832958944941148, 7711675412243671912, 15576564987190227975, 16452118100082038016, 6305011443818121839, 1213047649942025563, 11816267669673208372,
7503259434831574869, 15784731923736995898, 11287385040381237006, 3425713581329221729, 1436697996605827430, 11591809733187859977, 16677985422973077821, 6078267261889762898,
16292555063049989498, 5851447209550246421, 1630020308903038241, 11939238787801010766, 11081681957373440841, 3090674103720225830, 7876300217750508306, 16023932746787097725,
1565932757744914716, 12003503911822413427, 16230825569204842823, 5913566482019610152, 7956607163135676207, 15944361922680361024, 11164346891352108916, 3008957496780927003,
14514072000185962306, 8809633696146542637, 4460922918905818905, 10287960411460399222, 12879331835779764593, 113391187501452830, 5059972605034426666, 17660565739912801861,
4525502569691853604, 10224187249629523019, 14576435430675780479, 8748148222884465680, 4980157760350383383, 17740628527280140920, 12797300839518981452, 195741594718114339,
13040162471224305931, 565687821211481700, 4644267821511264592, 17536326748496696895, 14926957942186653496, 8937808626997553239, 4297282312656885603, 9839608450464401420,
4852190599768102253, 17327666750234135042, 13245728566574478646, 359174499151456857, 4073138765762497374, 10063573324157604913, 14700457781105076997, 9163920108173816938,
3628518000046490576, 9328460452529085631, 14330211790445699979, 8498696072880078052, 5299565100954197475, 18061012165519327884, 13623353920925351352, 1018284691440624343,
14265876314291404726, 8562713237611094233, 3566469078572851181, 9390260331795218562, 13702854325316886917, 937907429353946858, 5381352128745865694, 17978417549248290481,
5746791986423309721, 18225777846762470134, 13494053915084326338, 606523824971012781, 3751629717415787434, 9745292510640121029, 13876787882151992305, 8338992711486538910,
13285957365033343487, 815010154451519120, 5540840978686720420, 18431906428167644875, 14101316135270172620, 8115412784602421411, 3978303581567838103, 9519354766961195256,
12527462061959317731, 2230461459452909452, 6439665917889882296, 16893009583564617687, 15423350824487343824, 7288217715337890239, 2490078880175191691, 10493603952060017124,
6520081235612152965, 16813546994155744234, 12610022887636243678, 2148641156328442801, 2426095299884051126, 10557972909709735385, 15361512820870335213, 7350228890552538498,
15006518869663149738, 7165105895222849989, 2649782550477098737, 10947027550912647582, 12362696414880903321, 1783234539286425590, 6851427162658443458, 17022309211647725485,
2873395993211654860, 10722532847870938531, 15232418832718623383, 6938393941075996152, 6642978682516671743, 17230443782969840528, 12156534523779525796, 1989151790783919051,
6263731030979658865, 16556202624882645790, 11702894419100492842, 1245039440087595845, 3260040617806076482, 11390642587947386157, 15688795063501830681, 7680756410435167606,
11622868312827688983, 1324891275238549368, 6181348207440451660, 16638201170595874595, 15752600435501016612, 7616209416359311691, 3321489341258335871, 11328242235714328848,
3131865515489829432, 10977756817953029463, 16137146508898304611, 7844397531750915340, 5811434156413844491, 16395372229761246052, 11827132964039220304, 1660744670629167935,
15913214326271352414, 8068573254449152305, 2905717078206922245, 11204220263579804010, 12035829987123708013, 1452858539103461122, 6017914993561854006, 16189773752444600153
};

/*
uint64_t poly = 0xC96C5795D7870F42;
uint64_t table[256];
void generate_table(uint_least64_t* table)
{
    for(size_t i=0; i<256; ++i)
    {
    	uint64_t crc = i;

    	for(size_t j=0; j<8; ++j)
    	{
            // is current coefficient set?
    		if(crc & 1)
            {
                // yes, then assume it gets zero'd (by implied x^64 coefficient of dividend)
                crc >>= 1;

                // and add rest of the divisor
    			crc ^= poly;
            }
    		else
    		{
    			// no? then move to next coefficient
    			crc >>= 1;
            }
    	}

        table[i] = crc;
    }
}
*/

uint_least64_t crc64(const unsigned char * buf, size_t len)
{
    uint_least64_t crc = 0;
    for(size_t i=0; i<len; ++i)
    {
	unsigned char index = buf[i] ^ crc;
        uint64_t lookup = crc64_table[index];

        crc >>= 8;
        crc ^= lookup;
    }
    return crc;
}
