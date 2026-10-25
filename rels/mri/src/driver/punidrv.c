#include "punidrv.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"
#include "mri_puni.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/mapdrv.h"
#include "memory.h"
#include "system.h"
#include "win/win_main.h"
#include "math_trig.h"
#include <dolphin/os.h>
#include <string.h>

#pragma pool_data off

#pragma force_active on
static int compare(const void* a, const void* b);
static f32 camGetRy(void);

// clang-format off
extend_data(72,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(73,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 sintbl[450] = {
    0x00000000, 0x3C8EF77F, 0x3D0EF241, 0x3D565E46,
    0x3D8EDC3C, 0x3DB27ED8, 0x3DD612C7, 0x3DF99674,
    0x3E0E835E, 0x3E20303C, 0x3E31D0C8, 0x3E43636F,
    0x3E54E6E2, 0x3E66598E, 0x3E77BA67, 0x3E8483ED,
    0x3E8D204B, 0x3E95B1C8, 0x3E9E377A, 0x3EA6B0D9,
    0x3EAF1D3F, 0x3EB77C03, 0x3EBFCC7D, 0x3EC80DE5,
    0x3ED03FD5, 0x3ED86163, 0x3EE0722A, 0x3EE87161,
    0x3EF05E81, 0x3EF83904, 0x3F000000, 0x3F03D988,
    0x3F07A8C6, 0x3F0B6D76, 0x3F0F2746, 0x3F12D5E0,
    0x3F167914, 0x3F1A108C, 0x3F1D9BF6, 0x3F211B1E,
    0x3F248DC1, 0x3F27F37C, 0x3F2B4C2B, 0x3F2E976C,
    0x3F31D51B, 0x3F3504F7, 0x3F3826AB, 0x3F3B3A04,
    0x3F3E3EC0, 0x3F41349C, 0x3F441B76, 0x3F46F30A,
    0x3F49BB17, 0x3F4C7358, 0x3F4F1BBD, 0x3F51B3F2,
    0x3F543BC5, 0x3F56B314, 0x3F5919AC, 0x3F5B6F4C,
    0x3F5DB3D0, 0x3F5FE719, 0x3F6208D1, 0x3F6418F8,
    0x3F66175D, 0x3F6803CD, 0x3F69DE16, 0x3F6BA637,
    0x3F6D5BEE, 0x3F6EFF19, 0x3F708FB8, 0x3F720D78,
    0x3F737868, 0x3F74D068, 0x3F761544, 0x3F7746ED,
    0x3F786552, 0x3F797050, 0x3F7A67E8, 0x3F7B4BE8,
    0x3F7C1C61, 0x3F7CD91F, 0x3F7D8234, 0x3F7E177F,
    0x3F7E98FE, 0x3F7F06A3, 0x3F7F605B, 0x3F7FA637,
    0x3F7FD817, 0x3F7FF60A, 0x3F800000, 0x3F7FF60A,
    0x3F7FD817, 0x3F7FA637, 0x3F7F605B, 0x3F7F06A3,
    0x3F7E98FE, 0x3F7E177F, 0x3F7D8234, 0x3F7CD91F,
    0x3F7C1C61, 0x3F7B4BE8, 0x3F7A67E8, 0x3F797050,
    0x3F786552, 0x3F7746ED, 0x3F761544, 0x3F74D068,
    0x3F737879, 0x3F720D88, 0x3F708FB8, 0x3F6EFF2A,
    0x3F6D5BEE, 0x3F6BA637, 0x3F69DE27, 0x3F6803CD,
    0x3F66175D, 0x3F641909, 0x3F6208E1, 0x3F5FE719,
    0x3F5DB3E1, 0x3F5B6F5D, 0x3F5919AC, 0x3F56B325,
    0x3F543BD6, 0x3F51B3F2, 0x3F4F1BBD, 0x3F4C7369,
    0x3F49BB17, 0x3F46F30A, 0x3F441B86, 0x3F4134AD,
    0x3F3E3EC0, 0x3F3B3A04, 0x3F3826AB, 0x3F3504F7,
    0x3F31D52C, 0x3F2E977D, 0x3F2B4C2B, 0x3F27F37C,
    0x3F248DC1, 0x3F211B2E, 0x3F1D9C06, 0x3F1A108C,
    0x3F167925, 0x3F12D5F1, 0x3F0F2746, 0x3F0B6D76,
    0x3F07A8D6, 0x3F03D999, 0x3F000000, 0x3EF83904,
    0x3EF05EA2, 0x3EE87182, 0x3EE0724B, 0x3ED86184,
    0x3ED03FD5, 0x3EC80E06, 0x3EBFCC7D, 0x3EB77C24,
    0x3EAF1D60, 0x3EA6B0FB, 0x3E9E377A, 0x3E95B1C8,
    0x3E8D206D, 0x3E84840E, 0x3E77BAAA, 0x3E6659D1,
    0x3E54E6E2, 0x3E43636F, 0x3E31D10B, 0x3E20307F,
    0x3E0E83A1, 0x3DF996FB, 0x3DD6134D, 0x3DB27ED8,
    0x3D8EDCC2, 0x3D565F52, 0x3D0EF34D, 0x3C8EF998,
    0x358637BD, 0xBC8EF77F, 0xBD0EF241, 0xBD565D3A,
    0xBD8EDC3C, 0xBDB27E52, 0xBDD612C7, 0xBDF99674,
    0xBE0E835E, 0xBE20303C, 0xBE31D0C8, 0xBE43632C,
    0xBE54E69F, 0xBE66594B, 0xBE77BA23, 0xBE8483ED,
    0xBE8D204B, 0xBE95B1A7, 0xBE9E3758, 0xBEA6B0D9,
    0xBEAF1D1D, 0xBEB77BE1, 0xBEBFCC5C, 0xBEC80DC3,
    0xBED03FB3, 0xBED86163, 0xBEE0722A, 0xBEE87161,
    0xBEF05E81, 0xBEF838E3, 0xBEFFFFDE, 0xBF03D977,
    0xBF07A8C6, 0xBF0B6D76, 0xBF0F2735, 0xBF12D5E0,
    0xBF167914, 0xBF1A107B, 0xBF1D9BF6, 0xBF211B1E,
    0xBF248DB0, 0xBF27F36B, 0xBF2B4C1B, 0xBF2E976C,
    0xBF31D51B, 0xBF3504E6, 0xBF38269A, 0xBF3B39F3,
    0xBF3E3EAF, 0xBF41349C, 0xBF441B76, 0xBF46F2FA,
    0xBF49BB06, 0xBF4C7358, 0xBF4F1BAC, 0xBF51B3F2,
    0xBF543BC5, 0xBF56B314, 0xBF5919AC, 0xBF5B6F4C,
    0xBF5DB3D0, 0xBF5FE708, 0xBF6208D1, 0xBF6418F8,
    0xBF66175D, 0xBF6803BC, 0xBF69DE16, 0xBF6BA626,
    0xBF6D5BDD, 0xBF6EFF19, 0xBF708FA8, 0xBF720D78,
    0xBF737868, 0xBF74D057, 0xBF761533, 0xBF7746ED,
    0xBF786541, 0xBF797050, 0xBF7A67D7, 0xBF7B4BE8,
    0xBF7C1C61, 0xBF7CD91F, 0xBF7D8234, 0xBF7E177F,
    0xBF7E98FE, 0xBF7F06A3, 0xBF7F605B, 0xBF7FA626,
    0xBF7FD817, 0xBF7FF60A, 0xBF800000, 0xBF7FF60A,
    0xBF7FD817, 0xBF7FA637, 0xBF7F605B, 0xBF7F06A3,
    0xBF7E98FE, 0xBF7E177F, 0xBF7D8234, 0xBF7CD91F,
    0xBF7C1C61, 0xBF7B4BE8, 0xBF7A67E8, 0xBF797050,
    0xBF786552, 0xBF7746ED, 0xBF761544, 0xBF74D068,
    0xBF737879, 0xBF720D88, 0xBF708FB8, 0xBF6EFF2A,
    0xBF6D5BEE, 0xBF6BA637, 0xBF69DE27, 0xBF6803CD,
    0xBF66176E, 0xBF641909, 0xBF6208E1, 0xBF5FE719,
    0xBF5DB3E1, 0xBF5B6F5D, 0xBF5919BD, 0xBF56B325,
    0xBF543BD6, 0xBF51B403, 0xBF4F1BCE, 0xBF4C7369,
    0xBF49BB17, 0xBF46F31B, 0xBF441B86, 0xBF4134AD,
    0xBF3E3ED1, 0xBF3B3A04, 0xBF3826BB, 0xBF350508,
    0xBF31D52C, 0xBF2E977D, 0xBF2B4C3C, 0xBF27F38C,
    0xBF248DC1, 0xBF211B2E, 0xBF1D9C06, 0xBF1A109D,
    0xBF167925, 0xBF12D5F1, 0xBF0F2757, 0xBF0B6D87,
    0xBF07A8D6, 0xBF03D999, 0xBF000011, 0xBEF83926,
    0xBEF05EC4, 0xBEE87182, 0xBEE0724B, 0xBED86184,
    0xBED03FD5, 0xBEC80E06, 0xBEBFCC9F, 0xBEB77C24,
    0xBEAF1D82, 0xBEA6B0FB, 0xBE9E379B, 0xBE95B1EA,
    0xBE8D206D, 0xBE84840E, 0xBE77BAAA, 0xBE6659D1,
    0xBE54E725, 0xBE4363B2, 0xBE31D10B, 0xBE20307F,
    0xBE0E83E4, 0xBDF99781, 0xBDD613D3, 0xBDB27F5F,
    0xBD8EDD48, 0xBD565F52, 0xBD0EF45A, 0xBC8EFBB1,
    0xB58637BD, 0x3C8EF566, 0x3D0EF241, 0x3D565D3A,
    0x3D8EDBB6, 0x3DB27DCC, 0x3DD61240, 0x3DF995EE,
    0x3E0E831B, 0x3E202FF9, 0x3E31D085, 0x3E43632C,
    0x3E54E65C, 0x3E66594B, 0x3E77BA23, 0x3E8483CB,
    0x3E8D2029, 0x3E95B1A7, 0x3E9E3737, 0x3EA6B0B8,
    0x3EAF1D1D, 0x3EB77BE1, 0x3EBFCC3A, 0x3EC80DC3,
    0x3ED03FB3, 0x3ED86141, 0x3EE07208, 0x3EE87161,
    0x3EF05E81, 0x3EF838E3, 0x3EFFFFDE, 0x3F03D977,
    0x3F07A8B5, 0x3F0B6D65, 0x3F0F2724, 0x3F12D5D0,
    0x3F167903, 0x3F1A107B, 0x3F1D9BF6, 0x3F211B0D,
    0x3F248DB0, 0x3F27F36B, 0x3F2B4C1B, 0x3F2E975B,
    0x3F31D50B, 0x3F3504D5, 0x3F38269A, 0x3F3B39F3,
    0x3F3E3EAF, 0x3F41349C, 0x3F441B76, 0x3F46F2FA,
    0x3F49BB06, 0x3F4C7358, 0x3F4F1BAC, 0x3F51B3E1,
    0x3F543BC5, 0x3F56B314, 0x3F59199C, 0x3F5B6F3B,
    0x3F5DB3D0, 0x3F5FE708, 0x3F6208D1, 0x3F6418F8,
    0x3F66174C, 0x3F6803BC, 0x3F69DE16, 0x3F6BA626,
    0x3F6D5BDD, 0x3F6EFF19, 0x3F708FA8, 0x3F720D78,
    0x3F737868, 0x3F74D057, 0x3F761533, 0x3F7746DC,
    0x3F786541, 0x3F797050, 0x3F7A67D7, 0x3F7B4BE8,
    0x3F7C1C61, 0x3F7CD91F, 0x3F7D8234, 0x3F7E177F,
    0x3F7E98FE, 0x3F7F06A3, 0x3F7F605B, 0x3F7FA626,
    0x3F7FD817, 0x3F7FF60A,
};

// Normals$480, Colors$481 and UVs$482 are static locals of puniEntry (below).

static PuniDrvWork* work;

void puniSinCosRet(u32 angle, f32* sinOut, f32* cosOut, f32 scaleSin, f32 scaleCos) {
    f32* tbl = (f32*)sintbl;
    f32* tbl2 = (f32*)sintbl;
    s32 idx = angle % 360;

    *sinOut += scaleSin * tbl[idx];
    *cosOut += scaleCos * tbl2[idx + 90];
}

void puniInit(void) {
    PuniDrvWork* wp = _mapAlloc(sizeof(PuniDrvWork));

    work = wp;
    wp->poseNum = 0;
    wp->poses = NULL;
    wp->fadeOff = 1;
}

void puniEntry(s32 count, void* param, void (*callback)(void), void (*cb0)(void), void (*cb1)(void)) {
    static Vec Normals[2] = {
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, -1.0f },
    };
    static GXColor Colors = { 0xFF, 0xFF, 0xFF, 0xFF };
    static f32 UVs[11] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f };
    PuniDrvWork* wp = work;
    PuniTexInfo* p = param;
    s32 texCount = 0;
    s32 i;

    while (p->name != NULL) {
        texCount++;
        p++;
    }

    wp->poses = __memAlloc(2, count * sizeof(PuniPose));
    wp->poseNum = count;
    wp->cb0 = cb0;
    wp->cb1 = cb1;
    wp->textures = __memAlloc(2, texCount * sizeof(PuniTexture));
    wp->texCount = texCount;
    wp->mtx = __memAlloc(2, count * 0x30);
    wp->vertices = __memAlloc(2, texCount * 0xC * sizeof(f32));
    wp->normals = Normals;
    wp->colors = &Colors;
    wp->uvs = UVs;
    wp->sortList = __memAlloc(2, count * 8);
    wp->dlBuffer = __memAlloc(2, texCount * 0x80);
    DCInvalidateRange(wp->dlBuffer, texCount * 0x80);
    wp->dlSizes = __memAlloc(2, texCount * 4);
    memset(wp->poses, 0, wp->poseNum * sizeof(PuniPose));

    for (i = 0; i < count; i++) {
        wp->sortList[i].poseId = i;
        wp->poses[i].matrix = &wp->mtx[i];
        wp->poses[i].scale = 1.0f;
    }

    PSMTXIdentity(wp->shadowMtx);
    dispEntry(CAMERA_OFFSCREEN, 0.0f, 1, puniMakeTexture, param);
    if (callback != NULL) {
        callback();
    }
}

void puniMakeTexture(CameraId camId, void* param) {
    PuniDrvWork* wp = work;
    PuniTexInfo* info = param;
    PuniTexture* tex = wp->textures;
    s32 i;
    // A named scale: as a literal it swaps FP registers with the int->float bias.
    f32 k = 0.34246576f;

    anim_ext_tex_size = 0;
    // The original bumps i, then info, then tex at the loop's end (a `for` puts i last).
    i = 0;
    while (i < wp->texCount) {
        s32 out0;
        s32 out1;
        s32 out2;
        s32 out3;
        s32 idx = i * 12;
        s32 vi = i * 4;

        animPoseDisp_MakeExtTexture(info->name, info->animName, info->frame, &tex->texObj, tex, 5.84f, &out0, &out1, &out2,
                                    &out3);
        wp->vertices[idx++] = (f32)out0 * k;
        wp->vertices[idx++] = (f32)(-out1) * k;
        wp->vertices[idx++] = 0.0f;
        wp->vertices[idx++] = (f32)out2 * k;
        wp->vertices[idx++] = (f32)(-out1) * k;
        wp->vertices[idx++] = 0.0f;
        wp->vertices[idx++] = (f32)out2 * k;
        wp->vertices[idx++] = (f32)(-out3) * k;
        wp->vertices[idx++] = 0.0f;
        wp->vertices[idx++] = (f32)out0 * k;
        wp->vertices[idx++] = (f32)(-out3) * k;
        wp->vertices[idx++] = 0.0f;

        GXBeginDisplayList(wp->dlBuffer[i], 0x80);
        GXBegin(0x80, 0, 4);
        {
            s32 p = vi;

            GXPosition1x16(p++);
            GXTexCoord1x16(0);
            GXPosition1x16(p++);
            GXTexCoord1x16(1);
            GXPosition1x16(p++);
            GXTexCoord1x16(2);
            GXPosition1x16(p++);
            GXTexCoord1x16(3);
        }
        wp->dlSizes[i] = GXEndDisplayList();
        i++;
        info++;
        tex++;
    }
}

void puniReset(void) {
    PuniDrvWork* wp = work;

    if (wp->poses != NULL) {
        __memFree(2, wp->poses);
    }
    if (wp->textures != NULL) {
        __memFree(2, wp->textures);
    }
    if (wp->mtx != NULL) {
        __memFree(2, wp->mtx);
    }
    if (wp->vertices != NULL) {
        __memFree(2, wp->vertices);
    }
    if (wp->sortList != NULL) {
        __memFree(2, wp->sortList);
    }
    if (wp->dlBuffer != NULL) {
        __memFree(2, wp->dlBuffer);
    }
    if (wp->dlSizes != NULL) {
        __memFree(2, wp->dlSizes);
    }
    memset(wp, 0, sizeof(PuniDrvWork));
}

void puniFadeOff(s32 fadeOff) {
    work->fadeOff = fadeOff;
}

void puniMain(void) {
    PuniDrvWork* wp = work;
    s32 i;
    PuniPose* pose;
    PuniSortEntry* sort;
    f32 angle;
    f32 target;

    if (winCheck() == 0 && wp->cb0 != NULL) {
        wp->cb0();
    }
    if (wp->cb1 != NULL) {
        wp->cb1();
    }
    pose = wp->poses;
    sort = wp->sortList;
    angle = camGetRy();
    camGetPtr(CAMERA_3D);
    camGetPtr(CAMERA_3D);
    target = 180.0f + angle;

    for (i = wp->poseNum - 1; i >= 0; i--) {
        u32 flags = pose->flags;

        sort->poseId = i;
        if (flags & PUNI_POSE_VISIBLE) {
            sort->key = (f32)pose->texId;
        }
        {
            f32 diff = 90.0f + (angle - pose->facing);

            while (diff < -360.0f) {
                diff += 360.0f;
            }
            while (diff >= 360.0f) {
                diff -= 360.0f;
            }
            if ((diff > 90.0f && diff <= 270.0f) || (diff < -90.0f && diff >= -270.0f)) {
                pose->billboardYawTarget = target;
            } else {
                pose->billboardYawTarget = camGetRy();
            }
        }
        while ((pose->billboardYawTarget - pose->billboardYaw) > 180.0f) {
            pose->billboardYaw += 360.0f;
        }
        while ((pose->billboardYawTarget - pose->billboardYaw) < -180.0f) {
            pose->billboardYaw -= 360.0f;
        }
        if (flags & PUNI_POSE_EASE_TURN) {
            f32 diff = pose->billboardYawTarget - pose->billboardYaw;

            pose->billboardYaw = 0.1f * diff + pose->billboardYaw;
        } else {
            pose->billboardYaw = pose->billboardYawTarget;
        }
        {
            f32 a = pose->billboardYaw;
            f32 c;
            f32 s;
            s32 idx;

            while (a < 0.0f) {
                a += 360.0f;
            }
            while (a >= 360.0f) {
                a -= 360.0f;
            }
            idx = (s32)a;
            s = ((f32*)sintbl)[idx];
            c = ((f32*)sintbl)[idx + 90];
            (*pose->matrix)[0][0] = c;
            (*pose->matrix)[2][2] = c;
            (*pose->matrix)[0][2] = s;
            (*pose->matrix)[2][0] = -s;
        }
        pose++;
        sort++;
    }

    if (wp->poseNum != 0) {
        qqsort(wp->sortList, wp->poseNum, 8, compare);
    }
    wp->lastTex = -1;
    wp->texSize = 0;
}

static int compare(const void* a, const void* b) {
    const PuniSortEntry* ea = a;
    const PuniSortEntry* eb = b;

    if (ea->key > eb->key) {
        return 1;
    }
    if (ea->key < eb->key) {
        return -1;
    }
    return 0;
}

PuniPose* puniGetPosePtr(void) {
    return work->poses;
}

s32 puniGetPoseNum(void) {
    return work->poseNum;
}

void puniPoseDraw(s32 poseId) {
    PuniDrvWork* wp = work;
    PuniPose* pose = &wp->poses[poseId];

    if (pose->flags & PUNI_POSE_VISIBLE) {
        s32 texId = pose->texId;

        if (wp->lastTex != texId) {
            PuniTexture* tex = &wp->textures[texId];

            GXLoadTexObjPreLoaded(smartTexObj(&tex->texObj, tex->image), &tex->region, 0);
            wp->lastTex = texId;
        }
        {
            Mtx m;

            PSMTXConcat(camGetCurPtr()->view, *pose->matrix, m);
            GXLoadPosMtxImm(m, 0);
            GXSetCurrentMtx(0);
            GXCallDisplayList(wp->dlBuffer[pose->texId], wp->dlSizes[pose->texId]);
        }
    }
}

void puniDraw(void) {
    PuniDrvWork* wp = work;
    s32 i;
    PuniSortEntry* sort = wp->sortList;

    for (i = wp->poseNum - 1; i >= 0; i--, sort++) {
        PuniPose* pose = &wp->poses[sort->poseId];

        if (pose->flags & PUNI_POSE_VISIBLE) {
            s32 texId = pose->texId;

            if (texId != wp->lastTex) {
                PuniTexture* tex = &wp->textures[texId];

                GXLoadTexObjPreLoaded(smartTexObj(&tex->texObj, tex->image), &tex->region, 0);
                wp->lastTex = texId;
            }
            {
                Mtx m;
                Mtx scale;
                Mtx rot;
                Mtx trans;

                PSMTXTrans(trans, 0.0f, pose->rotOffsetY, 0.0f);
                PSMTXRotRad(rot, 'Z', 0.017453292f * pose->rotZ);
                PSMTXScale(scale, pose->scale, pose->scale, 1.0f);
                PSMTXConcat(*pose->matrix, trans, m);
                PSMTXConcat(m, rot, m);
                PSMTXConcat(m, scale, m);
                PSMTXConcat(camGetCurPtr()->view, m, m);
                GXLoadPosMtxImm(m, 0);
                GXSetCurrentMtx(0);
                GXCallDisplayList(wp->dlBuffer[texId], wp->dlSizes[texId]);
            }
        }
    }
}

void puniLoadRenderMode(void) {
    GXSetBlendMode(0, 1, 0, 0);
    GXSetZCompLoc(0);
    GXSetAlphaCompare(6, 0x80, 1, 0, 0);
    GXSetZMode(1, 3, 1);
}

void puniLoadVertex(void) {
    PuniDrvWork* wp = work;

    GXClearVtxDesc();
    GXSetVtxDesc(9, 3);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetArray(9, wp->vertices, 0xC);
    GXSetVtxDesc(0xD, 3);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXSetArray(0xD, wp->uvs, 8);
    GXSetCullMode(0);
}

void puniLoadTexture(void) {
    PuniDrvWork* wp = work;
    PuniTexture* tex = wp->textures;
    s32 i;
    u32 total = 0;

    GXInvalidateTexAll();
    for (i = 0; i < wp->texCount; i++, tex++) {
        u32 size = GXGetTexBufferSize(GXGetTexObjWidth(&tex->texObj), GXGetTexObjHeight(&tex->texObj),
                                      GXGetTexObjFmt(&tex->texObj), 0, 0);

        if (total < 0x80000 && total + size >= 0x80000) {
            total = 0x80000;
        }
        GXInitTexPreLoadRegion(&tex->region, total, size, 0, 0);
        GXPreLoadEntireTexture(smartTexObj(&tex->texObj, tex->image), &tex->region);
        total += size;
    }
    wp->texSize = total;
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
}

u32 puniGetUsedTmemSize(void) {
    return work->texSize;
}

void puniLoadTextureExit(void) {
    GXTexModeSync();
}

// original global data referenced by puniLoadTev / puniLoadShadowTev
__declspec(section ".rodata") GXColor dat_mri_000252f0 = { 0xFF, 0xFF, 0xFF, 0xFF };
__declspec(section ".rodata") GXColor dat_mri_000252f4 = { 0xFF, 0xFF, 0xFF, 0xFF };
__declspec(section ".rodata") GXColor dat_mri_000252f8 = { 0xFF, 0xFF, 0xFF, 0xFF };

void puniLoadTev(void) {
    GXColor blend;
    GXColor color = dat_mri_000252f8;

    if (work->fadeOff == 0) {
        mapGetBlend(&blend);
        GXSetTevColor(1, blend);
        GXSetTevOrder(0, 0, 0, 0xFF);
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0, 0xF, 8, 2, 0xF);
        GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(0, 7, 4, 1, 7);
    } else {
        GXSetTevOrder(0, 0, 0, 0xFF);
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
        GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(0, 7, 7, 7, 4);
    }
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, color);
    GXSetNumTexGens(1);
    GXSetNumTevStages(1);
}

static f32 camGetRy(void) {
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    f32 angle = (f32)atan2(-(cam->cameraAt.x - cam->cameraPos.x), -(cam->cameraAt.z - cam->cameraPos.z));

    angle = 57.29579f * angle;
    while (angle >= 360.0f) {
        angle -= 360.0f;
    }
    if (angle < 0.0f) {
        angle += 360.0f;
    }
    return angle;
}

// original global data referenced by puniLoadShadowTev
__declspec(section ".rodata") GXColor dat_mri_00025300 = { 0, 0, 0, 0x40 };

// TODO: type
static u32 shadow_dl[8] = {
    0xA0001200, 0x01020304, 0x05060708, 0x090A0B0C,
    0x0D0E0F10, 0x01000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 shadow_vtx[56] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x3F800000, 0x3F3504F7, 0x00000000,
    0x3F3504F7, 0x3F800000, 0x00000000, 0x00000000,
    0x3F3504F7, 0x00000000, 0xBF3504E6, 0x358637BD,
    0x00000000, 0xBF800000, 0xBF3504E6, 0x00000000,
    0xBF3504F7, 0xBF800000, 0x00000000, 0xB58637BD,
    0xBF350508, 0x00000000, 0x3F3504E6, 0xB58637BD,
    0x00000000, 0x3F800000, 0x3F3504D5, 0x00000000,
    0x3F350508, 0x3F800000, 0x00000000, 0x360637BD,
    0x3F350508, 0x00000000, 0xBF3504D5, 0x360637BD,
    0x00000000, 0xBF800000, 0xBF3504D5, 0x00000000,
    0xBF350518, 0xBF800000, 0x00000000, 0xB60637BD,
    0xBF350508, 0x00000000, 0x3F3504D5, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 shadow_col[8] = {
    0x00000080, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

void puniLoadShadowVertex(void) {
    GXClearVtxDesc();
    GXSetVtxDesc(9, 2);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetArray(9, shadow_vtx, 0xC);
    GXSetCullMode(0);
}

void puniLoadShadowTev(void) {
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
    GXSetChanMatColor(4, dat_mri_00025300);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0xFF, 0xFF, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
    GXSetTevAlphaIn(0, 7, 7, 7, 5);
}

void puniLoadShadowRenderMode(void) {
    GXSetBlendMode(1, 4, 5, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZMode(1, 3, 0);
}

void puniLoadShadowMtx(Mtx mtx) {
    PSMTXCopy(mtx, work->shadowMtx);
}

void puniDrawShadow(void) {
    PuniDrvWork* wp = work;
    s32 i;
    PuniSortEntry* sort = &wp->sortList[wp->poseNum - 1];

    for (i = wp->poseNum - 1; i >= 0; i--, sort--) {
        PuniPose* pose = &wp->poses[sort->poseId];

        if (!(pose->flags & PUNI_POSE_NO_SHADOW)) {
            Mtx m;

            PSMTXConcat(camGetCurPtr()->view, *pose->matrix, m);
            PSMTXConcat(m, wp->shadowMtx, m);
            if (pose->flags & PUNI_POSE_SHADOW_Y) {
                PSMTXTransApply(m, m, 0.0f, -(*pose->matrix)[1][3] + pose->shadowY, 0.0f);
            }
            GXLoadPosMtxImm(m, 0);
            GXSetCurrentMtx(0);
            GXCallDisplayList(shadow_dl, 0x20);
        }
    }
}
// clang-format on
