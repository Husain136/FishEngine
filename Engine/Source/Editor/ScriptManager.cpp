#include "ScriptManager.hpp"
#include <memory>

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif
#include <boost/dll/import.hpp>

#include <Application.hpp>
#include <Debug.hpp>

#include <EditorApplication.hpp>


using namespace FishEngine;
using namespace FishEditor;

struct ScriptManagerImpl
{
	Path sharedLibPath;

	boost::dll::detail::library_function<Script*(const char*)> createFunc;
	boost::dll::detail::library_function<void(Script *)> deleteFunc;

	ScriptManagerImpl(Path const & path)
		: sharedLibPath(path),
		createFunc(boost::dll::import<Script*(const char*)>(path, "CreateCustomScript")),
		deleteFunc(boost::dll::import<void(Script*)>(path, "DestroyCustomScript"))
	{
	}
};


ScriptManager::ScriptManager()
{
	BuildScriptsInProject();
	auto project_name = Application::dataPath().parent_path().stem().string();
#if FISHENGINE_PLATFORM_WINDOWS
	auto path = Format("../build/%1%/%2%.dll", CMAKE_INTDIR, project_name);
#else
	auto path = Format("../build/%1%/lib%2%.dylib", CMAKE_INTDIR, project_name);
#endif
	auto sharedLibPath = Application::dataPath() / path;
	if (!boost::filesystem::exists(sharedLibPath))
	{
		LogError(sharedLibPath.string() + " not found");
		abort();
	}
	m_impl = new ScriptManagerImpl(sharedLibPath);
}

ScriptManager::~ScriptManager()
{
	delete m_impl;
}

ScriptPtr ScriptManager::CreateScript(std::string const & name)
{
	auto s = std::shared_ptr<Script>(m_impl->createFunc(name.c_str()), [this](auto s) {
		LogInfo("Delete from dll");
		m_impl->deleteFunc(s);
	});
	m_scripts.push_back(s);
	return s;
}

void FishEditor::ScriptManager::BuildScriptsInProject()
{
	try {
		auto python_lib_paths = {
			L"/Users/yushroom/Downloads/Python-3.6.1/Lib",	// sys lib
			L"/Users/yushroom/.pyenv/versions/3.6.1/lib/python3.6/site-packages", // mako
			L"/Users/yushroom/program/FishEngine/Script/Editor", // editor
			L"/Users/yushroom/Downloads/Python-3.6.1/debug/build/lib.macosx-10.12-x86_64-3.6-pydebug" // sys lib
		};
		std::wstring path;
		for (auto & p : python_lib_paths)
		{
			path += p;
			path += L":";
		}
		Py_SetPath(path.c_str());
		Py_Initialize();
		PyRun_SimpleString("import os\nfrom mako.template import Template\nprint('Hello Python')\n");
		auto pName = PyUnicode_DecodeFSDefault("BuildProject");
		auto pModule = PyImport_Import(pName);
		assert(pModule != nullptr);
		auto pFunc = PyObject_GetAttrString(pModule, "buildProject");
		if (!pFunc || !PyCallable_Check(pFunc))
			abort();
		auto buildPath = Application::dataPath().parent_path();
		auto pBuildPath = PyUnicode_DecodeFSDefault(buildPath.string().c_str());
		auto pBuildType = PyUnicode_DecodeFSDefault(CMAKE_INTDIR);
		auto cmakePath = EditorApplication::applicationPath() / "Tools/cmake/bin/cmake";
		auto pCmakePath = PyUnicode_DecodeFSDefault(cmakePath.string().c_str());
		auto pArgs = PyTuple_New(3);
		PyTuple_SetItem(pArgs, 0, pBuildPath);
		PyTuple_SetItem(pArgs, 1, pBuildType);
		PyTuple_SetItem(pArgs, 2, pCmakePath);
		PyObject_CallObject(pFunc, pArgs);
//		Py_DECREF(pCmakePath);
//		Py_DECREF(pBuildType);
//		Py_DECREF(pBuildPath);
		Py_DECREF(pArgs);
		Py_DECREF(pFunc);
		Py_DECREF(pModule);
		Py_DECREF(pName);
		Py_Finalize();
	}
	catch (std::exception & e)
	{
		LogError(e.what());
		abort();
	}
}
