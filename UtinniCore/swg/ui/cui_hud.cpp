#include "cui_hud.h"
#include "swg/ui/imgui_impl.h"
#include "swg/misc/swg_misc.h"
#include "swg/camera/camera.h"
#include "swg/scene/client_world.h"
#include "swg/scene/ground_scene.h"
#include "swg/game/game.h"

namespace swg::cuiHud
{
using pUpdate = void(__thiscall*)(swgptr pThis, float time);
using pActionPerformAction = bool(__thiscall*)(swgptr pThis, DWORD val1, DWORD val2);
using pGetTarget = utinni::Object* (__cdecl*)(utinni::Camera* pCamera, math::Vector* worldStart, math::Vector* worldEnd, utinni::Object* obj);

pUpdate update = (pUpdate)0x00BD56F0;
pActionPerformAction actionPerformAction = (pActionPerformAction)0x00EDBAA0;
pGetTarget getTarget = (pGetTarget)0x00BD3E20;

}

namespace utinni::cuiHud
{
Object* targetUnderCursor;

void __fastcall hkUpdate(swgptr pThis, float time)
{
    swg::cuiHud::update(pThis, time);
}

bool __fastcall hkActionPerformAction(swgptr pThis, DWORD EDX, DWORD val1, DWORD val2)
{
    if (imgui_gizmo::hasMouseHover())
    {
        return false;
    }

    return swg::cuiHud::actionPerformAction(pThis, val1, val2);
}

bool collideCursorWithWorld(int x, int y, swg::math::Vector& result, Object* excludeObject)
{
    Camera* camera = GroundScene::get()->getCurrentCamera();
    swg::math::Vector worldStart = camera->getTransform_o2w()->getPosition();
    swg::math::Vector viewDirection = camera->getTransform_o2w()->rotate_l2p(camera->reverseProjectInViewportSpace(x - camera->viewportX, y - camera->viewportY));
    viewDirection.normalize();

    const float viewDistance = memory::read<float>(0x19488C8); // 0x19488C8 = Static location of view distance
    swg::math::Vector worldEnd = worldStart + viewDirection * viewDistance;

    static constexpr uint16_t flags = (1 | 128 | 4096); // terrain | child objects | disable portal crossing
    CollisionInfo collisionResults;
    auto tit = camera->getParentCell();
    if (clientWorld::collide(tit, &worldStart, &worldEnd, collisionResults, flags, excludeObject))
    {
        result = swg::math::Vector(collisionResults.point);
        return true;
    }
    return false;
}

static swg::math::Vector cursorWorldPos;
Object* __cdecl hkGetTarget(Camera* pCamera, swg::math::Vector* worldStart, swg::math::Vector* worldEnd, Object* obj)
{
    static constexpr uint16_t flags = (1 | 128 | 4096); // terrain | child objects | disable portal crossing
    CollisionInfo collisionResults;
    if (clientWorld::collide(pCamera->getParentCell(), worldStart, worldEnd, collisionResults, flags, obj))
    {
        cursorWorldPos = swg::math::Vector(collisionResults.point);
    }
    return swg::cuiHud::getTarget(pCamera, worldStart, worldEnd, obj);
}

static constexpr swgptr jmpToTarget_midUpdate = 0x00BD5961;
static constexpr swgptr return_midUpdate = 0x00BD595C;
__declspec(naked) void midUpdate()
{
    swgptr pTargetUnderCursor;
    __asm
    {
        mov pTargetUnderCursor, ecx
        pushad
        pushfd
    }

    targetUnderCursor = (Object*)pTargetUnderCursor;
    
    __asm
    {
        popfd
        popad
        test eax, eax
        jz jumpToTarget
        mov targetUnderCursor, 0
        push edi
        lea ecx, [0x84 + eax]
        jmp[return_midUpdate]

        jumpToTarget:
        jmp[jmpToTarget_midUpdate]
    }
}

const swg::math::Vector& getCursorWorldPosition()
{
    return cursorWorldPos;
}

void patchAllowTargetEverything(bool value)
{
    if (value)
    {
        memory::createJMP(0x00BD3FA3, 0x00BD403D, 5);
    }
    else
    {
        static constexpr byte originalBytes[] = { 0x8B, 0xCE, 0xE8, 0x76, 0xF9 }; // call client.9C18A0
        memory::copy(0x00BD3FA3, originalBytes);
    }
}

void detour()
{
    //swg::cuiHud::update = (swg::cuiHud::pUpdate)Detour::Create((LPVOID)swg::cuiHud::update, hkUpdate, DETOUR_TYPE_PUSH_RET);
    swg::cuiHud::actionPerformAction = (swg::cuiHud::pActionPerformAction)Detour::Create((LPVOID)swg::cuiHud::actionPerformAction, hkActionPerformAction, DETOUR_TYPE_PUSH_RET);
    swg::cuiHud::getTarget = (swg::cuiHud::pGetTarget)Detour::Create((LPVOID)swg::cuiHud::getTarget, hkGetTarget, DETOUR_TYPE_PUSH_RET, 5);

    //memory::createJMP(0x00BD5951, (swgptr)midUpdate, 6); // Mid CuiHud::update detour
}

}
