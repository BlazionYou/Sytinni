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

#include "game.h"
#include "utinni.h"
#include "utility/memory.h"
#include <imgui/imgui_user.h>
#include "swg/client/client.h"
#include "swg/misc/config.h"
#include "swg/scene/ground_scene.h"
#include "swg/scene/world_snapshot.h"
#include "swg/object/client_object.h"
#include "swg/object/object.h"
#include "swg/ui/imgui_impl.h"
#include "utility/log.h"
#include "ini/ini.h"

namespace swg::game
{
using pInstall = void(__cdecl*)(int applicationType);
using pQuit = void(__cdecl*)();
using pMainLoop = void(__cdecl*)(bool presentToWindow, HWND hwnd, int width, int height);

using pSetupScene = void(__cdecl*)(utinni::GroundScene* newScene);
using pCleanupScene = void(__cdecl*)();

using pGetPlayer = utinni::Object* (__cdecl*)();
using pGetPlayerCreatureObject = utinni::Object* (__cdecl*)();

using pGetCamera = utinni::Camera* (__cdecl*)();
using pGetConstCamera = const utinni::Camera* (__cdecl*)();

using pIsViewFirstPerson = bool(__cdecl*)();
using pIsHudSceneTypeSpace = bool(__cdecl*)();

pInstall install = (pInstall)0x00422E80;
pQuit quit = (pQuit)0x00423720;
pMainLoop mainLoop = (pMainLoop)0x004237C0;

pSetupScene setupScene = (pSetupScene)0x00424220;
pCleanupScene cleanupScene = (pCleanupScene)0x00423700;

pGetPlayer getPlayer = (pGetPlayer)0x00425140;
pGetPlayerCreatureObject getPlayerCreatureObject = (pGetPlayerCreatureObject)0x004251D0;

pGetCamera getCamera = (pGetCamera)0x00425BB0;
pGetConstCamera getConstCamera = (pGetConstCamera)0x00425BE0;

pIsViewFirstPerson isViewFirstPerson = (pIsViewFirstPerson)0x00425C10;
pIsHudSceneTypeSpace isHudSceneTypeSpace = (pIsHudSceneTypeSpace)0x00426170;
}

static std::vector<void(*)()> installCallbacks;
static std::vector<void(*)()> preMainLoopCallbacks;
static std::vector<void(*)()> mainLoopCallbacks;
static std::vector<void(*)()> setSceneCallbacks;
static std::vector<void(*)()> cleanUpSceneCallbacks;
static std::unique_ptr<utinni::Repository> repository;

namespace utinni
{

void Game::addInstallCallback(void(*func)())
{
    installCallbacks.emplace_back(func);
}

void Game::addPreMainLoopCallback(void(*func)())
{
    preMainLoopCallbacks.emplace_back(func);
}

void Game::addMainLoopCallback(void(*func)())
{
    mainLoopCallbacks.emplace_back(func);
}

void Game::addSetSceneCallback(void(*func)())
{
    setSceneCallbacks.emplace_back(func);
}

void Game::addCleanupSceneCallback(void(*func)())
{
    cleanUpSceneCallbacks.emplace_back(func);
}

int getMainLoopCount()
{
    return memory::read<int>(0x1908830); // Ptr to the main loop count
}

bool loadNewScene = false;
bool sceneCleaned = false;
std::string sceneToLoadTerrainFilename;
std::string sceneToLoadAvatarObjectFilename = "object/creature/player/shared_human_male.iff";
void __cdecl hkMainLoop(bool presentToWindow, HWND hwnd, int width, int height)
{
    for (const auto& func : preMainLoopCallbacks)
    {
        func();
    }

    swg::game::mainLoop(presentToWindow, hwnd, width, height);    

    for (const auto& func : mainLoopCallbacks)
    {
        func();
    }

    if (loadNewScene && sceneCleaned)
    {
        loadNewScene = false;
        sceneCleaned = false;
        swg::game::setupScene(GroundScene::ctor(sceneToLoadTerrainFilename.c_str(), sceneToLoadAvatarObjectFilename.c_str()));
    }

    if (loadNewScene)
    {
        Game::cleanupScene();
        sceneCleaned = true;
    }
}

void __cdecl hkInstall(int application)
{
    swg::game::install(application);

    repository = std::unique_ptr<Repository>();
    WorldSnapshot::generateHighestId();

    for (const auto& func : installCallbacks)
    {
        func();
    }

    if (getConfig().getBool("UtinniCore", "autoLoadScene"))
    {
        Game::loadScene();
    }
}

void __cdecl hkSetScene(GroundScene* scene)
{
    swg::game::setupScene(scene);

    if (scene != nullptr)
    {
        for (const auto& func : setSceneCallbacks)
        {
            func();
        }
    }
}

void __cdecl hkCleanupScene()
{
    swg::game::cleanupScene();

    imgui_gizmo::disable();

    for (const auto& func : cleanUpSceneCallbacks)
    {
        func();
    }
}

void Game::detour()
{
    if (getMainLoopCount() == 0) // Checks the Games main loop count, if 0, we're in the 'suspended' startup entry point loop
    {
        //utility::showMessageBox("");
        swg::game::mainLoop = (swg::game::pMainLoop)Detour::Create(swg::game::mainLoop, hkMainLoop, DETOUR_TYPE_PUSH_RET);
        swg::game::install = (swg::game::pInstall)Detour::Create(swg::game::install, hkInstall, DETOUR_TYPE_PUSH_RET);
        swg::game::setupScene = (swg::game::pSetupScene)Detour::Create(swg::game::setupScene, hkSetScene, DETOUR_TYPE_PUSH_RET);
        swg::game::cleanupScene = (swg::game::pCleanupScene)Detour::Create(swg::game::cleanupScene, hkCleanupScene, DETOUR_TYPE_PUSH_RET);
    }
}

void Game::quit()
{
    swg::game::quit();
}

bool Game::isRunning()
{
    return getMainLoopCount();
}

void Game::loadScene()
{
    const char* terrainFilename = swg::config::clientGame::getSceneTerrainFilename();
    const char* avatarFilename = swg::config::clientGame::getSceneAvatarFilename();

    if (terrainFilename != nullptr)
    {
        sceneToLoadTerrainFilename = terrainFilename;
    }

    if (avatarFilename != nullptr)
    {
        sceneToLoadAvatarObjectFilename = avatarFilename;
    }

    if (sceneToLoadTerrainFilename.empty())
    {
        log::error("Failed to load scene due to there being no set terrain filename.");
        return;
    }

    loadNewScene = true;
}

void Game::loadScene(const char* terrainFilename, const char* avatarObjectFilename)
{
    sceneToLoadTerrainFilename = terrainFilename;
    sceneToLoadAvatarObjectFilename = avatarObjectFilename;
    loadNewScene = true;
}

void Game::cleanupScene()
{
    swg::game::cleanupScene();
}

Repository* Game::getRepository()
{
    return repository.get();
}

Object* Game::getPlayer()
{
    return swg::game::getPlayer();
}

Object* Game::getPlayerCreatureObject() // ToDo return CreatureObject*
{
    return swg::game::getPlayerCreatureObject();
}

swgptr Game::getPlayerLookAtTargetObjectNetworkId()
{
    const Object* playerObj = getPlayerCreatureObject();

    if (!playerObj)
    {
        return 0;
    }

    return (swgptr)playerObj + 1432;
}

Object* Game::getPlayerLookAtTargetObject()
{
    const swgptr lookAtId = getPlayerLookAtTargetObjectNetworkId();

    if (lookAtId == 0)
    {
        return nullptr;
    }

    return Object::getObjectById(lookAtId);
}

Camera* Game::getCamera()
{
    return swg::game::getCamera();
}

const Camera* Game::getConstCamera()
{
    return swg::game::getConstCamera();
}


bool Game::isSafeToUse()
{
    return memory::read<bool>(0x01908858) || memory::read<bool>(0x01919410);
}

}
