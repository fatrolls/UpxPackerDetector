char *upx_signatures[]= {
	"60E8000000005883E83D", 
		"[UPX0.50-0.70]",
	"60E80000000083CDFF31DB5E", 
		"[UPX0.72]",
	"5E89F7B9????????8A07472CE83C0177F7803F??75F28B078A5F0466C1E808C1C01086C429F880EBE801F0890783C70588D8E2D98D??????????8B0709C0743C8B5F048D????????????01F35083C708FF??????????958A074708C074DC89F95748F2AE55FF??????????09C07407890383C304EBE1FF??????????8BAE????????8DBE00F0FFFFBB0010000050546A045357FFD58D87????????80207F8060287F585054505357FFD558618D4424806A0039C475FA83EC80E9", 
		"[UPX2.00-3.0X->MarkusOberhumer&LaszloMolnar&JohnReiser]",
	"60BE????????8DBE????????C787????????????????5783CDFF89E58D9C24????????31C05039DC75FB46465368????????5783C3045368????????5683C304", 
		"[UPX2.90[LZMA](Delphistub)->MarkusOberhumer,LaszloMolnar&JohnReiser]",
	"60BE????????8DBE????????5783CDFF89E58D9C24????????31C05039DC75FB46465368????????5783C3045368????????5683C3045350C703????????9090", 
		"[UPX2.90[LZMA]->MarkusOberhumer,LaszloMolnar&JohnReiser]",
	"60BE????????8DBE????????5783CDFFEB109090909090908A064688074701DB75078B1E83EEFC11DB72EDB80100000001DB75078B1E83EEFC11DB11C001DB", 
		"[UPX2.90[LZMA]->MarkusOberhumer,LaszloMolnar&JohnReiser]",
	"60BE????????8DBE????????5789E58D9C24????????31C05039DC75FB46465368????????5783C3045368????????5683C3045350C703030002009090909090",
		"[UPX2.93-3.00[LZMA]->MarkusOberhumer,LaszloMolnar&JohnReiser]",
	"01DB078B1E83EEFC11DBEDB80100000001DB078B1E83EEFC11DB11C001DB730B",
		"[UPXAlternativestub]",
	"9C60E8000000005DB8B38540002DAC8540002BE88DB5D5FEFFFF8B0683F80074118DB5E1FEFFFF8B0683F8010F84F1010000C706010000008BD58B85B1FEFFFF2BD08995B1FEFFFF0195C9FEFFFF8DB5E5FEFFFF01",
		"[UPXInlinerv1.0byGPcH]",
	"79070FB707475047B95748F2AE55FF9684??000009C07407890383C304EBD8FF9688??000061E9??????FF",
		"[UPXModifiedstub]",
	"60BE????????8DBE????????5783CDFFFCB28031DBA4B302E86D00000073F631C9E864000000731C31C0E85B0000007323B30241B010E84F00000010C073F7753FAAEBD4E84D00000029D97510E842000000EB28AC",
		"[UPXModifiedStubb->Farb-rauschConsumerConsulting]",
	"60BE????????8DBE????????5783CDFFFCB280E8000000005B83C366A4FFD373FB31C9FFD3731431C0FFD3731D41B010FFD310C073FA753CAAEBE2E84A00000049E210E840000000EB28ACD1E8744511C9EB1C9148",
		"[UPXModifiedStubc->Farb-rauschConsumerConsulting]",
	"50BE????????8DBE????????5783CD",
		"[UPXModifierv0.1x]",
	"EBEC????????8A064688074701DB7507",
		"[UPXProtectorv1.0x]",
	"EB??????????8A064688074701DB75078B1E83EEFC11DB",
		"[UPXProtectorv1.0x(2)]",
	"????????????????????????????????????????????????8A064688074701DB75078B1E83EEFC11DB8A0772EBB80100000001DB75078B1E83EEFC11DB11C001DB73??75??8B1E83EEFC",
		"[UPXv1.03-v1.04]",
	"01DB??078B1E83EEFC11DB8A07??EBB80100000001DB??078B1E83EEFC11DB11C001DB73EF",
		"[UPXv1.03-v1.04Modified]",
	"60BE????????8DBE????????5783CDFFEB05A401DB75078B1E83EEFC11DB72F231C04001DB75078B1E83EEFC11DB11C001DB75078B1E83EEFC11DB73E631C983",
		"[Upxv1.2->Marcus&Lazlo]",
	"FFD580A7??????????585054505357FFD558618D4424??6A0039C475FA83EC80E9",
		"[UPXV1.94->MarkusOberhumer&LaszloMolnar&JohnReiser]",
	"55FF96????????09C07407890383C304EB??FF96????????8BAE????????8DBE00F0FFFFBB0010000050546A045357FFD58D87????000080207F8060287F585054505357FFD558618D4424806A0039C475FA83EC80",
		"[UPXv2.0->Markus,Laszlo&Reiser]",
	"FFD58D87????????8020??8060????585054505357FFD558618D4424??6A0039C475FA83EC80E9",
		"[UPXV2.00-V2.90->MarkusOberhumer&LaszloMolnar&JohnReiser]",
	"60BE??????008DBE??????FF57",
		"[UPXv3.0->Markus,Laszlo&Reiser]",
	"E2FA94FFE06100000000000000",
		"[UPX$HiT0.0.1->DJSiba]",
	"94BC??????00B9??00000080340C??E2FA94FFE061",
		"[UPX$HiTv0.0.1->DJSiba]",
	"60E8000000005D81ED4812400060E82B03000061",
		"[Upx-Lock1.0-1.2-->CyberDoom/Team-X&BoB/BobSoft]",
	"E8000000005983C10751C3C3BE????????83EC04893424B9800000008136????????50B80400000050033424585883E903E2E9EBD6",
		"[UPX-SCRAMBLER3.06->�OnT�oL]",
	"9061BE????????8DBE????????5783CDFF",
		"[UPX-ScramblerRCv1.x]",
	"E8000000005E83C614AD89C7AD89C1AD300747E2FBADFFE0C300????00??????00??????01??????005550582D536869742076302E31202D207777772E626C61636B6C6F6769632E6E6574202D20636F6465206279",
		"[UPX-Shitv0.1->500mhz]",
	"E8000000005E83C614AD89C7AD89C1AD300747E2FBADFFE0C300????00??????00??????????????005550582D536869742076302E31202D207777772E626C61636B6C6F6769632E6E6574202D20636F6465206279",
		"[UPX-Shitv0.1->500mhz]",
	"E8????????5E83C6??AD89C7AD89C1AD300747E2??ADFFE0C3",
		"[UPX-Shitv0.1->500mhz]",
	"BF??????0081FF??????007410812F??00000083C704BB05????00FFE3BE??????00FFE600000000",
		"[UPXcrypter->archphase/NWC]",
	"BE????????83C601FFE6000000??????0003000000????????001000000000????????0000??F6??00B24F4500??F9??00EF4F4500??F6??008CD14200??56??00??????00??????00??????00??24??00??????00",
		"[UPXFreakv0.1(BorlandDelphi)->HMX0101]",
	"BE????????83C601FFE6000000??????0003000000????????001000000000????????0000??F6??00B24F4500??F9??00EF4F4500??F6??008CD14200??56??00??????00??????00??????00??24??00??????0034504500??????00FFFF0000??24??00??24??00??????00400000C00000????????0000??000000??1E??00??F7??00A64E4300??56??00ADD14200??F7??00A1D24200??56??000B4D4300??F7??00??F7??00??56??00??????????000000??????????????77??????00??????00??????77????0000??????00????????????0000??????00??????????????????????00????????00000000??????00",
		"[UPXFreakv0.1(BorlandDelphi)->HMX0101]",
	"BE????????83C601FFE60000",
		"[UPXFreakV0.1->HMX0101]",
	"B8????4300B915000000803408??E2FAE9D6FFFFFF",
		"[UPXShit0.06]"
};