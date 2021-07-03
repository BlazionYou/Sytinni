/**
 * MIT License
 *
 * Copyright (c) 2020 Philip Klatt
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
**/

#include "appearance.h"
#include "swg/graphics/graphics.h"
#include "swg/ui/cui_hud.h"
#include "utility/memory.h"
#include "utility/log.h"
#include "utility/utility.h"

namespace swg::appearance
{
using pCreateAppearance = utinni::Appearance* (__cdecl*)(const char* filename);
using pCollide = bool (__stdcall*)(swg::math::Vector* worldStart, swg::math::Vector* worldEnd, swgptr unk, utinni::CollisionInfo& collisionResults);

pCreateAppearance createAppearance = (pCreateAppearance)0x00B262A0;
pCollide collide = (pCollide)0x00B2C410;
}

namespace swg::particleEffectAppearance
{
using pCtor = swgptr(__thiscall*)(utinni::ParticleEffectAppearance* pThis, swgptr particleEffectAppearanceTemplate);
using pRender = void(__thiscall*)(utinni::ParticleEffectAppearance* pThis);

pCtor ctor = (pCtor)0x007A85A0;
pRender render = (pRender)0x007A8A50;
}

namespace utinni
{
Appearance* Appearance::create(const char* filename)
{
    return swg::appearance::createAppearance(filename);
}

BoxExtent* Appearance::getExtent()
{
    return (BoxExtent*)(((swgptr)this) + 0x6C); // ToDo BoxExtent is not for all Appearances, change
}

bool ParticleEffectAppearance::collide(swg::math::Vector* worldStart, swg::math::Vector* worldEnd, swgptr unk, utinni::CollisionInfo& collisionResults)
{
    if (getExtent() != nullptr)
    {
        swg::math::Vector normal;
        float time = 0;
        if (getExtent()->realIntersect(cuiHud::getWs(), cuiHud::getWe(), &normal, &time))
        {
            collisionResults.point.X = cuiHud::getWs()->X + (cuiHud::getWe()->X - cuiHud::getWs()->X) * time;
            collisionResults.point.Y = cuiHud::getWs()->Y + (cuiHud::getWe()->Y - cuiHud::getWs()->Y) * time;
            collisionResults.point.Z = cuiHud::getWs()->Z + (cuiHud::getWe()->Z - cuiHud::getWs()->Z) * time;
            collisionResults.normal = normal;
            collisionResults.time = time;

            log::info(appearanceTemplate->m_crcName->buffer);

            return true;
        }
    }

    return false;
}

swgptr __fastcall hkCtor(ParticleEffectAppearance* pThis, swgptr EDX, swgptr particleEffectAppearanceTemplate)
{
    swgptr result = swg::particleEffectAppearance::ctor(pThis, particleEffectAppearanceTemplate);

    // Override the vtbl collide function with our own
    memory::patchAddress(memory::read<swgptr>(result) + 0x0C, (swgptr)utility::union_cast<void*>(&ParticleEffectAppearance::collide));
    return result;
}

void  __fastcall hkRender(ParticleEffectAppearance* pThis)
{
    //swg::particleEffectAppearance::render(pThis);
    Graphics::setStaticShader(memory::read<swgptr>(0x1922F8C));
    Graphics::setObjectToWorldTransformAndScale((swg::math::Transform*)0x1945AD4, (swg::math::Vector*)0x194596C);
    Graphics::drawExtent(pThis->getExtent(), 0x1945A0C);

}

void ParticleEffectAppearance::detour()
{
    //swg::particleEffectAppearance::ctor = (swg::particleEffectAppearance::pCtor)Detour::Create(swg::particleEffectAppearance::ctor, hkCtor, DETOUR_TYPE_PUSH_RET);
    //swg::particleEffectAppearance::render = (swg::particleEffectAppearance::pRender)Detour::Create(swg::particleEffectAppearance::render, hkRender, 5);
}


}
