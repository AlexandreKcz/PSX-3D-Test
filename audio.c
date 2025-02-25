#include <libspu.h>

#define SOUND_MALLOC_MAX 10

SpuCommonAttr l_c_attr;
SpuVoiceAttr g_s_attr;
unsigned long l_vag1_spu_addr;

void audio_init() {
    SpuInit();
    SpuInitMalloc (SOUND_MALLOC_MAX, (char*)(SPU_MALLOC_RECSIZ * (SOUND_MALLOC_MAX + 1)));
    l_c_attr.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR);
    l_c_attr.mvol.left = 0x3fff;
    l_c_attr.mvol.right = 0x3fff;
    SpuSetCommonAttr(&l_c_attr);
}

void audio_transfer_vag_to_spu(char* sound, int sound_size, int voice_channel) {
    SpuSetTransferMode(SpuTransferByDMA);
    l_vag1_spu_addr = SpuMalloc(sound_size);
    SpuSetTransStartAddr(l_vag1_spu_addr);
    SpuWrite(sound + 0x30, sound_size);
    SpuIsTransferCompleted(SPU_TRANSFER_WAIT);
    g_s_attr.mask = (
        			SPU_VOICE_VOLL |
					SPU_VOICE_VOLR |
					SPU_VOICE_PITCH |
					SPU_VOICE_WDSA |
					SPU_VOICE_ADSR_AMODE |
					SPU_VOICE_ADSR_SMODE |
					SPU_VOICE_ADSR_RMODE |
					SPU_VOICE_ADSR_AR |
					SPU_VOICE_ADSR_DR |
					SPU_VOICE_ADSR_SR |
					SPU_VOICE_ADSR_RR |
					SPU_VOICE_ADSR_SL
    );

    g_s_attr.voice = (voice_channel);

    g_s_attr.volume.left = 0x1fff;
    g_s_attr.volume.right = 0x1fff;

    g_s_attr.pitch        = 0x1000;
	g_s_attr.addr         = l_vag1_spu_addr;
	g_s_attr.a_mode       = SPU_VOICE_LINEARIncN;
	g_s_attr.s_mode       = SPU_VOICE_LINEARIncN;
	g_s_attr.r_mode       = SPU_VOICE_LINEARDecN;
	g_s_attr.ar           = 0x0;
	g_s_attr.dr           = 0x0;
	g_s_attr.sr           = 0x0;
	g_s_attr.rr           = 0x0;
	g_s_attr.sl           = 0xf;

    SpuSetVoiceAttr (&g_s_attr);
}

void audio_play(int voice_channel) {
    SpuSetKey(SpuOn, voice_channel);
}

void audio_free(unsigned long spu_address) {
    SpuFree(spu_address);
}
