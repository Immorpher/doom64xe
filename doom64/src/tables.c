#include "doomdef.h"

fixed_t     finesine[10240]; // 800A8210

const angle_t tantoangle[2049] = { // 8005b894
0,333772,667544,1001315,1335086,1668857,2002626,2336395,
2670163,3003929,3337694,3671457,4005219,4338979,4672736,5006492,
5340245,5673995,6007743,6341488,6675230,7008968,7342704,7676435,
8010164,8343888,8677609,9011325,9345037,9678744,10012447,10346145,
10679838,11013526,11347209,11680887,12014558,12348225,12681885,13015539,
13349187,13682829,14016464,14350092,14683714,15017328,15350936,15684536,
16018129,16351714,16685291,17018860,17352422,17685974,18019518,18353054,
18686582,19020100,19353610,19687110,20020600,20354080,20687552,21021014,
21354466,21687906,22021338,22354758,22688168,23021568,23354956,23688332,
24021698,24355052,24688396,25021726,25355046,25688352,26021648,26354930,
26688200,27021456,27354702,27687932,28021150,28354356,28687548,29020724,
29353888,29687038,30020174,30353296,30686404,31019496,31352574,31685636,
32018684,32351718,32684734,33017736,33350722,33683692,34016648,34349584,
34682508,35015412,35348300,35681172,36014028,36346868,36679688,37012492,
37345276,37678044,38010792,38343524,38676240,39008936,39341612,39674272,
40006912,40339532,40672132,41004716,41337276,41669820,42002344,42334848,
42667332,42999796,43332236,43664660,43997060,44329444,44661800,44994140,
45326456,45658752,45991028,46323280,46655512,46987720,47319908,47652072,
47984212,48316332,48648428,48980500,49312548,49644576,49976580,50308556,
50640512,50972444,51304352,51636236,51968096,52299928,52631740,52963524,
53295284,53627020,53958728,54290412,54622068,54953704,55285308,55616888,
55948444,56279972,56611472,56942948,57274396,57605816,57937212,58268576,
58599916,58931228,59262512,59593768,59924992,60256192,60587364,60918508,
61249620,61580704,61911760,62242788,62573788,62904756,63235692,63566604,
63897480,64228332,64559148,64889940,65220696,65551424,65882120,66212788,
66543420,66874024,67204600,67535136,67865648,68196120,68526568,68856984,
69187360,69517712,69848024,70178304,70508560,70838776,71168960,71499112,
71829224,72159312,72489360,72819376,73149360,73479304,73809216,74139096,
74468936,74798744,75128520,75458264,75787968,76117632,76447264,76776864,
77106424,77435952,77765440,78094888,78424304,78753688,79083032,79412336,
79741608,80070840,80400032,80729192,81058312,81387392,81716432,82045440,
82374408,82703336,83032224,83361080,83689896,84018664,84347400,84676096,
85004760,85333376,85661952,85990488,86318984,86647448,86975864,87304240,
87632576,87960872,88289128,88617344,88945520,89273648,89601736,89929792,
90257792,90585760,90913688,91241568,91569408,91897200,92224960,92552672,
92880336,93207968,93535552,93863088,94190584,94518040,94845448,95172816,
95500136,95827416,96154648,96481832,96808976,97136080,97463136,97790144,
98117112,98444032,98770904,99097736,99424520,99751256,100077944,100404592,
100731192,101057744,101384248,101710712,102037128,102363488,102689808,103016080,
103342312,103668488,103994616,104320696,104646736,104972720,105298656,105624552,
105950392,106276184,106601928,106927624,107253272,107578872,107904416,108229920,
108555368,108880768,109206120,109531416,109856664,110181872,110507016,110832120,
111157168,111482168,111807112,112132008,112456856,112781648,113106392,113431080,
113755720,114080312,114404848,114729328,115053760,115378136,115702464,116026744,
116350960,116675128,116999248,117323312,117647320,117971272,118295176,118619024,
118942816,119266560,119590248,119913880,120237456,120560984,120884456,121207864,
121531224,121854528,122177784,122500976,122824112,123147200,123470224,123793200,
124116120,124438976,124761784,125084528,125407224,125729856,126052432,126374960,
126697424,127019832,127342184,127664472,127986712,128308888,128631008,128953072,
129275080,129597024,129918912,130240744,130562520,130884232,131205888,131527480,
131849016,132170496,132491912,132813272,133134576,133455816,133776992,134098120,
134419184,134740176,135061120,135382000,135702816,136023584,136344272,136664912,
136985488,137306016,137626464,137946864,138267184,138587456,138907664,139227808,
139547904,139867920,140187888,140507776,140827616,141147392,141467104,141786752,
142106336,142425856,142745312,143064720,143384048,143703312,144022512,144341664,
144660736,144979744,145298704,145617584,145936400,146255168,146573856,146892480,
147211040,147529536,147847968,148166336,148484640,148802880,149121056,149439152,
149757200,150075168,150393072,150710912,151028688,151346400,151664048,151981616,
152299136,152616576,152933952,153251264,153568496,153885680,154202784,154519824,
154836784,155153696,155470528,155787296,156104000,156420624,156737200,157053696,
157370112,157686480,158002768,158318976,158635136,158951216,159267232,159583168,
159899040,160214848,160530592,160846256,161161840,161477376,161792832,162108208,
162423520,162738768,163053952,163369040,163684080,163999040,164313936,164628752,
164943504,165258176,165572784,165887312,166201776,166516160,166830480,167144736,
167458912,167773008,168087040,168400992,168714880,169028688,169342432,169656096,
169969696,170283216,170596672,170910032,171223344,171536576,171849728,172162800,
172475808,172788736,173101600,173414384,173727104,174039728,174352288,174664784,
174977200,175289536,175601792,175913984,176226096,176538144,176850096,177161984,
177473792,177785536,178097200,178408784,178720288,179031728,179343088,179654368,
179965568,180276704,180587744,180898720,181209616,181520448,181831184,182141856,
182452448,182762960,183073408,183383760,183694048,184004240,184314368,184624416,
184934400,185244288,185554096,185863840,186173504,186483072,186792576,187102000,
187411344,187720608,188029808,188338912,188647936,188956896,189265760,189574560,
189883264,190191904,190500448,190808928,191117312,191425632,191733872,192042016,
192350096,192658096,192966000,193273840,193581584,193889264,194196848,194504352,
194811792,195119136,195426400,195733584,196040688,196347712,196654656,196961520,
197268304,197574992,197881616,198188144,198494592,198800960,199107248,199413456,
199719584,200025616,200331584,200637456,200943248,201248960,201554576,201860128,
202165584,202470960,202776256,203081456,203386592,203691632,203996592,204301472,
204606256,204910976,205215600,205520144,205824592,206128960,206433248,206737456,
207041584,207345616,207649568,207953424,208257216,208560912,208864512,209168048,
209471488,209774832,210078112,210381296,210684384,210987408,211290336,211593184,
211895936,212198608,212501184,212803680,213106096,213408432,213710672,214012816,
214314880,214616864,214918768,215220576,215522288,215823920,216125472,216426928,
216728304,217029584,217330784,217631904,217932928,218233856,218534704,218835472,
219136144,219436720,219737216,220037632,220337952,220638192,220938336,221238384,
221538352,221838240,222138032,222437728,222737344,223036880,223336304,223635664,
223934912,224234096,224533168,224832160,225131072,225429872,225728608,226027232,
226325776,226624240,226922608,227220880,227519056,227817152,228115168,228413088,
228710912,229008640,229306288,229603840,229901312,230198688,230495968,230793152,
231090256,231387280,231684192,231981024,232277760,232574416,232870960,233167440,
233463808,233760096,234056288,234352384,234648384,234944304,235240128,235535872,
235831504,236127056,236422512,236717888,237013152,237308336,237603424,237898416,
238193328,238488144,238782864,239077488,239372016,239666464,239960816,240255072,
240549232,240843312,241137280,241431168,241724960,242018656,242312256,242605776,
242899200,243192512,243485744,243778896,244071936,244364880,244657744,244950496,
245243168,245535744,245828224,246120608,246412912,246705104,246997216,247289216,
247581136,247872960,248164688,248456320,248747856,249039296,249330640,249621904,
249913056,250204128,250495088,250785968,251076736,251367424,251658016,251948512,
252238912,252529200,252819408,253109520,253399536,253689456,253979280,254269008,
254558640,254848176,255137632,255426976,255716224,256005376,256294432,256583392,
256872256,257161024,257449696,257738272,258026752,258315136,258603424,258891600,
259179696,259467696,259755600,260043392,260331104,260618704,260906224,261193632,
261480960,261768176,262055296,262342320,262629248,262916080,263202816,263489456,
263776000,264062432,264348784,264635024,264921168,265207216,265493168,265779024,
266064784,266350448,266636000,266921472,267206832,267492096,267777264,268062336,
268347312,268632192,268916960,269201632,269486208,269770688,270055072,270339360,
270623552,270907616,271191616,271475488,271759296,272042976,272326560,272610048,
272893440,273176736,273459936,273743040,274026048,274308928,274591744,274874432,
275157024,275439520,275721920,276004224,276286432,276568512,276850528,277132416,
277414240,277695936,277977536,278259040,278540448,278821728,279102944,279384032,
279665056,279945952,280226752,280507456,280788064,281068544,281348960,281629248,
281909472,282189568,282469568,282749440,283029248,283308960,283588544,283868032,
284147424,284426720,284705920,284985024,285264000,285542912,285821696,286100384,
286378976,286657440,286935840,287214112,287492320,287770400,288048384,288326240,
288604032,288881696,289159264,289436768,289714112,289991392,290268576,290545632,
290822592,291099456,291376224,291652896,291929440,292205888,292482272,292758528,
293034656,293310720,293586656,293862496,294138240,294413888,294689440,294964864,
295240192,295515424,295790560,296065600,296340512,296615360,296890080,297164704,
297439200,297713632,297987936,298262144,298536256,298810240,299084160,299357952,
299631648,299905248,300178720,300452128,300725408,300998592,301271680,301544640,
301817536,302090304,302362976,302635520,302908000,303180352,303452608,303724768,
303996800,304268768,304540608,304812320,305083968,305355520,305626944,305898272,
306169472,306440608,306711616,306982528,307253344,307524064,307794656,308065152,
308335552,308605856,308876032,309146112,309416096,309685984,309955744,310225408,
310494976,310764448,311033824,311303072,311572224,311841280,312110208,312379040,
312647776,312916416,313184960,313453376,313721696,313989920,314258016,314526016,
314793920,315061728,315329408,315597024,315864512,316131872,316399168,316666336,
316933408,317200384,317467232,317733984,318000640,318267200,318533632,318799968,
319066208,319332352,319598368,319864288,320130112,320395808,320661408,320926912,
321192320,321457632,321722816,321987904,322252864,322517760,322782528,323047200,
323311744,323576192,323840544,324104800,324368928,324632992,324896928,325160736,
325424448,325688096,325951584,326215008,326478304,326741504,327004608,327267584,
327530464,327793248,328055904,328318496,328580960,328843296,329105568,329367712,
329629760,329891680,330153536,330415264,330676864,330938400,331199808,331461120,
331722304,331983392,332244384,332505280,332766048,333026752,333287296,333547776,
333808128,334068384,334328544,334588576,334848512,335108352,335368064,335627712,
335887200,336146624,336405920,336665120,336924224,337183200,337442112,337700864,
337959552,338218112,338476576,338734944,338993184,339251328,339509376,339767296,
340025120,340282848,340540480,340797984,341055392,341312704,341569888,341826976,
342083968,342340832,342597600,342854272,343110848,343367296,343623648,343879904,
344136032,344392064,344648000,344903808,345159520,345415136,345670656,345926048,
346181344,346436512,346691616,346946592,347201440,347456224,347710880,347965440,
348219872,348474208,348728448,348982592,349236608,349490528,349744320,349998048,
350251648,350505152,350758528,351011808,351264992,351518048,351771040,352023872,
352276640,352529280,352781824,353034272,353286592,353538816,353790944,354042944,
354294880,354546656,354798368,355049952,355301440,355552800,355804096,356055264,
356306304,356557280,356808128,357058848,357309504,357560032,357810464,358060768,
358311008,358561088,358811104,359060992,359310784,359560480,359810048,360059520,
360308896,360558144,360807296,361056352,361305312,361554144,361802880,362051488,
362300032,362548448,362796736,363044960,363293056,363541024,363788928,364036704,
364284384,364531936,364779392,365026752,365274016,365521152,365768192,366015136,
366261952,366508672,366755296,367001792,367248192,367494496,367740704,367986784,
368232768,368478656,368724416,368970080,369215648,369461088,369706432,369951680,
370196800,370441824,370686752,370931584,371176288,371420896,371665408,371909792,
372154080,372398272,372642336,372886304,373130176,373373952,373617600,373861152,
374104608,374347936,374591168,374834304,375077312,375320224,375563040,375805760,
376048352,376290848,376533248,376775520,377017696,377259776,377501728,377743584,
377985344,378227008,378468544,378709984,378951328,379192544,379433664,379674688,
379915584,380156416,380397088,380637696,380878176,381118560,381358848,381599040,
381839104,382079072,382318912,382558656,382798304,383037856,383277280,383516640,
383755840,383994976,384233984,384472896,384711712,384950400,385188992,385427488,
385665888,385904160,386142336,386380384,386618368,386856224,387093984,387331616,
387569152,387806592,388043936,388281152,388518272,388755296,388992224,389229024,
389465728,389702336,389938816,390175200,390411488,390647680,390883744,391119712,
391355584,391591328,391826976,392062528,392297984,392533312,392768544,393003680,
393238720,393473632,393708448,393943168,394177760,394412256,394646656,394880960,
395115136,395349216,395583200,395817088,396050848,396284512,396518080,396751520,
396984864,397218112,397451264,397684288,397917248,398150080,398382784,398615424,
398847936,399080320,399312640,399544832,399776928,400008928,400240832,400472608,
400704288,400935872,401167328,401398720,401629984,401861120,402092192,402323136,
402553984,402784736,403015360,403245888,403476320,403706656,403936896,404167008,
404397024,404626944,404856736,405086432,405316032,405545536,405774912,406004224,
406233408,406462464,406691456,406920320,407149088,407377760,407606336,407834784,
408063136,408291392,408519520,408747584,408975520,409203360,409431072,409658720,
409886240,410113664,410340992,410568192,410795296,411022304,411249216,411476032,
411702720,411929312,412155808,412382176,412608480,412834656,413060736,413286720,
413512576,413738336,413964000,414189568,414415040,414640384,414865632,415090784,
415315840,415540800,415765632,415990368,416215008,416439552,416663968,416888288,
417112512,417336640,417560672,417784576,418008384,418232096,418455712,418679200,
418902624,419125920,419349120,419572192,419795200,420018080,420240864,420463552,
420686144,420908608,421130976,421353280,421575424,421797504,422019488,422241344,
422463104,422684768,422906336,423127776,423349120,423570400,423791520,424012576,
424233536,424454368,424675104,424895744,425116288,425336736,425557056,425777280,
425997408,426217440,426437376,426657184,426876928,427096544,427316064,427535488,
427754784,427974016,428193120,428412128,428631040,428849856,429068544,429287168,
429505664,429724064,429942368,430160576,430378656,430596672,430814560,431032352,
431250048,431467616,431685120,431902496,432119808,432336992,432554080,432771040,
432987936,433204736,433421408,433637984,433854464,434070848,434287104,434503296,
434719360,434935360,435151232,435367008,435582656,435798240,436013696,436229088,
436444352,436659520,436874592,437089568,437304416,437519200,437733856,437948416,
438162880,438377248,438591520,438805696,439019744,439233728,439447584,439661344,
439875008,440088576,440302048,440515392,440728672,440941824,441154880,441367872,
441580736,441793472,442006144,442218720,442431168,442643552,442855808,443067968,
443280032,443492000,443703872,443915648,444127296,444338880,444550336,444761696,
444972992,445184160,445395232,445606176,445817056,446027840,446238496,446449088,
446659552,446869920,447080192,447290400,447500448,447710432,447920320,448130112,
448339776,448549376,448758848,448968224,449177536,449386720,449595808,449804800,
450013664,450222464,450431168,450639776,450848256,451056640,451264960,451473152,
451681248,451889248,452097152,452304960,452512672,452720288,452927808,453135232,
453342528,453549760,453756864,453963904,454170816,454377632,454584384,454791008,
454997536,455203968,455410304,455616544,455822688,456028704,456234656,456440512,
456646240,456851904,457057472,457262912,457468256,457673536,457878688,458083744,
458288736,458493600,458698368,458903040,459107616,459312096,459516480,459720768,
459924960,460129056,460333056,460536960,460740736,460944448,461148064,461351584,
461554976,461758304,461961536,462164640,462367680,462570592,462773440,462976160,
463178816,463381344,463583776,463786144,463988384,464190560,464392608,464594560,
464796448,464998208,465199872,465401472,465602944,465804320,466005600,466206816,
466407904,466608896,466809824,467010624,467211328,467411936,467612480,467812896,
468013216,468213440,468413600,468613632,468813568,469013440,469213184,469412832,
469612416,469811872,470011232,470210528,470409696,470608800,470807776,471006688,
471205472,471404192,471602784,471801312,471999712,472198048,472396288,472594400,
472792448,472990400,473188256,473385984,473583648,473781216,473978688,474176064,
474373344,474570528,474767616,474964608,475161504,475358336,475555040,475751648,
475948192,476144608,476340928,476537184,476733312,476929376,477125344,477321184,
477516960,477712640,477908224,478103712,478299104,478494400,478689600,478884704,
479079744,479274656,479469504,479664224,479858880,480053408,480247872,480442240,
480636512,480830656,481024736,481218752,481412640,481606432,481800128,481993760,
482187264,482380704,482574016,482767264,482960416,483153472,483346432,483539296,
483732064,483924768,484117344,484309856,484502240,484694560,484886784,485078912,
485270944,485462880,485654720,485846464,486038144,486229696,486421184,486612576,
486803840,486995040,487186176,487377184,487568096,487758912,487949664,488140320,
488330880,488521312,488711712,488901984,489092160,489282240,489472256,489662176,
489851968,490041696,490231328,490420896,490610336,490799712,490988960,491178144,
491367232,491556224,491745120,491933920,492122656,492311264,492499808,492688256,
492876608,493064864,493253056,493441120,493629120,493817024,494004832,494192544,
494380160,494567712,494755136,494942496,495129760,495316928,495504000,495691008,
495877888,496064704,496251424,496438048,496624608,496811040,496997408,497183680,
497369856,497555936,497741920,497927840,498113632,498299360,498484992,498670560,
498856000,499041376,499226656,499411840,499596928,499781920,499966848,500151680,
500336416,500521056,500705600,500890080,501074464,501258752,501442944,501627040,
501811072,501995008,502178848,502362592,502546240,502729824,502913312,503096704,
503280000,503463232,503646368,503829408,504012352,504195200,504377984,504560672,
504743264,504925760,505108192,505290496,505472736,505654912,505836960,506018944,
506200832,506382624,506564320,506745952,506927488,507108928,507290272,507471552,
507652736,507833824,508014816,508195744,508376576,508557312,508737952,508918528,
509099008,509279392,509459680,509639904,509820032,510000064,510180000,510359872,
510539648,510719328,510898944,511078432,511257856,511437216,511616448,511795616,
511974688,512153664,512332576,512511392,512690112,512868768,513047296,513225792,
513404160,513582432,513760640,513938784,514116800,514294752,514472608,514650368,
514828064,515005664,515183168,515360608,515537952,515715200,515892352,516069440,
516246432,516423328,516600160,516776896,516953536,517130112,517306592,517482976,
517659264,517835488,518011616,518187680,518363648,518539520,518715296,518891008,
519066624,519242144,519417600,519592960,519768256,519943424,520118528,520293568,
520468480,520643328,520818112,520992800,521167392,521341888,521516320,521690656,
521864896,522039072,522213152,522387168,522561056,522734912,522908640,523082304,
523255872,523429376,523602784,523776096,523949312,524122464,524295552,524468512,
524641440,524814240,524986976,525159616,525332192,525504640,525677056,525849344,
526021568,526193728,526365792,526537760,526709632,526881440,527053152,527224800,
527396352,527567840,527739200,527910528,528081728,528252864,528423936,528594880,
528765760,528936576,529107296,529277920,529448480,529618944,529789344,529959648,
530129856,530300000,530470048,530640000,530809888,530979712,531149440,531319072,
531488608,531658080,531827488,531996800,532166016,532335168,532504224,532673184,
532842080,533010912,533179616,533348288,533516832,533685312,533853728,534022048,
534190272,534358432,534526496,534694496,534862400,535030240,535197984,535365632,
535533216,535700704,535868128,536035456,536202720,536369888,536536992,536704000,
536870912
};
