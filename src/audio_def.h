#define XA_LENGTH(x) (((u64)(x) * 75) / 100 * IO_SECT_SIZE) //Centiseconds to sectors in bytes (w)

typedef struct
{
	XA_File file;
	u32 length;
} XA_TrackDef;

static const XA_TrackDef xa_tracks[] = {
	//MENU.XA
	{XA_Menu, XA_LENGTH(4705)}, //XA_GettinFreaky
	{XA_Menu, XA_LENGTH(7680)},  //XA_GameOver
	//MENU.XA
	{XA_GameOvr, XA_LENGTH(7680)}, //XA_GameOverJ
	{XA_GameOvr, XA_LENGTH(2461)},  //XA_GameOverP
	{XA_GameOvr, XA_LENGTH(3657)},  //XA_GameOverH
	//17.XA
	{XA_17, XA_LENGTH(12639)}, //XA_SussyBussy
	{XA_17, XA_LENGTH(11610)}, //XA_Rivals
	//18.XA
	{XA_18, XA_LENGTH(9598)}, //XA_Chewmate
	{XA_18, XA_LENGTH(9660)}, //XA_TomongusTuesday
	//19.XA
	{XA_19, XA_LENGTH(8657)}, //XA_Christmas
	{XA_19, XA_LENGTH(13521)}, //XA_Spookpostor
	//20.XA
	{XA_20, XA_LENGTH(10472)}, //XA_Titular
	{XA_20, XA_LENGTH(11000)}, //XA_GreatestPlan
	//21.XA
	{XA_21, XA_LENGTH(12819)}, //XA_Reinforcements
	{XA_21, XA_LENGTH(17092)}, //XA_Armed
	//22.XA
	{XA_22, XA_LENGTH(18723)}, //XA_AlphaMoogus
	{XA_22, XA_LENGTH(12777)}, //XA_ActinSus
	//23.XA
	{XA_23, XA_LENGTH(7885)}, //XA_Ow
	{XA_23, XA_LENGTH(11810)}, //XA_Who
	//24.XA
	{XA_24, XA_LENGTH(11311)}, //XA_InsaneStreamer
	{XA_24, XA_LENGTH(16600)}, //XA_SussusNuzzus
	//25.XA
	{XA_25, XA_LENGTH(10209)}, //XA_Idk
	{XA_25, XA_LENGTH(22378)}, //XA_Esculent
	//26.XA
	{XA_26, XA_LENGTH(16294)}, //XA_Drippypop
	{XA_26, XA_LENGTH(16324)}, //XA_Crewicide
	//27.XA
	{XA_27, XA_LENGTH(16541)}, //XA_MonotoneAttack
	{XA_27, XA_LENGTH(10338)}, //XA_Top10
	//28.XA
	{XA_28, XA_LENGTH(7885)}, //XA_Chippin
	{XA_28, XA_LENGTH(7909)}, //XA_Chipping
	//29.XA
	{XA_29, XA_LENGTH(12254)}, //XA_Torture
	//30.XA
	{XA_30, XA_LENGTH(9965)}, //XA_LemonLime
	{XA_30, XA_LENGTH(13714)}, //XA_Chlorophyll
	//31.XA
	{XA_31, XA_LENGTH(12745)}, //XA_Inflorescence
	{XA_31, XA_LENGTH(14000)}, //XA_Stargazer
};

static const char *xa_paths[] = {
	"\\MUSIC\\MENU.XA;1",   //XA_Menu
	"\\MUSIC\\GAMEOVR.XA;1",   //XA_GameOvr
	"\\MUSIC\\17.XA;1", //XA_17
	"\\MUSIC\\18.XA;1", //XA_18
	"\\MUSIC\\19.XA;1", //XA_19
	"\\MUSIC\\20.XA;1", //XA_20
	"\\MUSIC\\21.XA;1", //XA_21
	"\\MUSIC\\22.XA;1", //XA_22
	"\\MUSIC\\23.XA;1", //XA_23
	"\\MUSIC\\24.XA;1", //XA_24
	"\\MUSIC\\25.XA;1", //XA_25
	"\\MUSIC\\26.XA;1", //XA_26
	"\\MUSIC\\27.XA;1", //XA_27
	"\\MUSIC\\28.XA;1", //XA_28
	"\\MUSIC\\29.XA;1", //XA_29
	"\\MUSIC\\30.XA;1", //XA_30
	"\\MUSIC\\31.XA;1", //XA_31
	NULL,
};

typedef struct
{
	const char *name;
	boolean vocal;
} XA_Mp3;

static const XA_Mp3 xa_mp3s[] = {
	//MENU.XA
	{"freaky", false},   //XA_GettinFreaky
	{"gameover", false}, //XA_GameOver
	//GAMEOVR.XA
	{"gameoverj", false},   //XA_GameOverJ
	{"gameoverp", false}, //XA_GameOverP
	{"gameoverh", false}, //XA_GameOverH
	//17.XA
	{"sussybussy", true}, //XA_SussyBussy
	{"rivals", true}, //XA_Rivals
	//18.XA
	{"chewmate", true}, //XA_Chewmate
	{"tomongustuesday", true}, //XA_TomongusTuesday
	//19.XA
	{"christmas", true}, //XA_Christmas
	{"spookpostor", true}, //XA_Spookpostor
	//20.XA
	{"titular", true}, //XA_Titular
	{"greatestplan", true}, //XA_GreatestPlan
	//21.XA
	{"reinforcements", true}, //XA_Reinforcements
	{"armed", true}, //XA_Armed
	//22.XA
	{"alphamoogus", true}, //XA_AlphaMoogus
	{"actinsus", true}, //XA_ActinSus
	//23.XA
	{"ow", true}, //XA_Ow
	{"who", true}, //XA_Who
	//24.XA
	{"insanestreamer", true}, //XA_InsaneStreamer
	{"sussusnuzzus", true}, //XA_SussusNuzzus
	//25.XA
	{"idk", true}, //XA_Idk
	{"esculent", true}, //XA_Esculent
	//26.XA
	{"drippypop", true}, //XA_Drippypop
	{"crewicide", true}, //XA_Crewicide
	//27.XA
	{"monotoneattack", true}, //XA_MonotoneAttack
	{"top10", false}, //XA_Top10
	//28.XA
	{"chippin", true}, //XA_Chippin
	{"chipping", true}, //XA_Chipping
	//29.XA
	{"torture", true}, //XA_Torture
	//30.XA
	{"lemonlime", true}, //XA_LemonLime
	{"chlorophyll", true}, //XA_Chlorophyll
	//31.XA
	{"inflorescence", true}, //XA_Inflorescence
	{"stargazer", true}, //XA_Stargazer
	
	{NULL, false}
};
