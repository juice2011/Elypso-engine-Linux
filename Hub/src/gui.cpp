//Copyright(C) 2024 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <Windows.h>
#include <ShlObj.h>
#include <iostream>
#include <fstream>
#include <filesystem>

//external
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//hub
#include "gui.hpp"
#include "main.hpp"

using std::cout;
using std::ifstream;
using std::ofstream;
using std::getline;
using std::ios;
using std::wstring;
using std::filesystem::is_empty;
using std::filesystem::exists;
using std::filesystem::directory_iterator;
using std::filesystem::remove_all;
using std::filesystem::remove;
using std::filesystem::rename;
using std::filesystem::copy;
using std::filesystem::create_directory;

void GUI::Initialize()
{
	cout << "Initializing ImGui...\n\n";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::SetCurrentContext(ImGui::GetCurrentContext());
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	PWSTR path;
	HRESULT result = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &path);
	if (SUCCEEDED(result))
	{
		wstring wPath(path);
		CoTaskMemFree(path); //free the allocated memory

		//get the required buffer size
		int size_needed = WideCharToMultiByte(
			CP_UTF8,
			0,
			wPath.c_str(),
			static_cast<int>(wPath.length()),
			NULL,
			0,
			NULL,
			NULL);

		//convert wide string to utf-8 encoded narrow string
		string narrowPath(size_needed, 0);
		WideCharToMultiByte(
			CP_UTF8,
			0,
			wPath.c_str(),
			static_cast<int>(wPath.length()),
			&narrowPath[0],
			size_needed,
			NULL,
			NULL);

		size_t pos = 0;
		string incorrectSlash = "\\";
		string correctSlash = "/";
		while ((pos = narrowPath.find(incorrectSlash, pos)) != string::npos)
		{
			narrowPath.replace(pos, incorrectSlash.length(), correctSlash);
			pos += correctSlash.length();
		}
		Core::docsPath = narrowPath + "/Elypso hub";
	}

	if (!exists(Core::docsPath))
	{
		create_directory(Core::docsPath);
	}

	Core::projectsFilePath = Core::docsPath.string() + "/projects.txt";
	if (!exists(Core::projectsFilePath))
	{
		ofstream projectsFile(Core::projectsFilePath);
		if (!projectsFile.is_open())
		{
			cout << "Error: Failed to create " << Core::projectsFilePath << "!\n\n";
		}
		projectsFile.close();
	}

	Core::configFilePath = Core::docsPath.string() + "/config.txt";
	if (!exists(Core::configFilePath))
	{
		ofstream configFile(Core::configFilePath);
		if (!configFile.is_open())
		{
			cout << "Error: Failed to create " << Core::configFilePath << "!\n\n";
		}
		configFile.close();
	}

	static string tempString = Core::docsPath.string() + "/imgui.ini";
	const char* customConfigPath = tempString.c_str();
	io.IniFilename = customConfigPath;

	ImGui_ImplGlfw_InitForOpenGL(Core::window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	string fontPath = Core::defaultPath.string() + "/files/fonts/coda/Coda-Regular.ttf";
	if (exists(fontPath))
	{
		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF((Core::defaultPath.string() + "/files/fonts/coda/Coda-Regular.ttf").c_str(), 16.0f);
	}
	else
	{
		cout << "Error: Font " << fontPath << " does not exist!\n\n";
	}

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();

	style.TabRounding = 6;
	style.FramePadding = ImVec2(6, 2);
	style.ItemSpacing = ImVec2(0, 5);
	io.FontGlobalScale = 1.5f;
}

void GUI::Render()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();

	ImGuiDockNodeFlags dockFlags =
		ImGuiDockNodeFlags_PassthruCentralNode;

	GUI::RenderPanels();
	GUI::RenderButtons();

	if (renderConfirmWindow) GUI::ConfirmRemove(confirmFileName, confirmFilePath);

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockFlags);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::RenderPanels()
{
	glfwGetFramebufferSize(Core::window, &framebufferWidth, &framebufferHeight);

	ImGuiWindowFlags mainWindowFlags =
		ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoSavedSettings;

	ImGui::Begin("Panels", NULL, mainWindowFlags);

	ImGui::SetWindowPos(ImVec2(300, 0));
	ImGui::SetWindowSize(ImVec2(static_cast<float>(framebufferWidth) - 300, static_cast<float>(framebufferHeight)));

	int panelHeightInt = static_cast<int>(panelHeight);
	int panelSpacingInt = static_cast<int>(panelSpacing);
	int height = (panelHeightInt + panelSpacingInt) * static_cast<int>(files.size());
	if (height < framebufferHeight - 20) height = framebufferHeight - 20;
	ImGui::BeginChild("ScrollingRegion", ImVec2(static_cast<float>(framebufferWidth), static_cast<float>(height)), true, ImGuiWindowFlags_HorizontalScrollbar);

	ImVec2 nextPanelPos = ImGui::GetCursorScreenPos();

	if (files.size() > 0)
	{
		for (const auto& file : files)
		{
			ImGuiWindowFlags windowFlags =
				ImGuiWindowFlags_NoCollapse
				| ImGuiWindowFlags_NoTitleBar
				| ImGuiWindowFlags_NoResize
				| ImGuiWindowFlags_NoMove
				| ImGuiWindowFlags_NoSavedSettings;

			ImGui::SetNextWindowPos(nextPanelPos);

			ImGui::Begin(file.c_str(), NULL, windowFlags);

			ImGui::SetWindowSize(ImVec2(static_cast<float>(framebufferWidth) - 335, 200));

			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)
				&& !renderConfirmWindow)
			{
				ImGui::SetWindowFocus();
			}

			string fileName = path(file).stem().string();
			ImGui::Text("%s", fileName.c_str());

			if (ImGui::Button("Launch", ImVec2(200, 50)))
			{
				if (!renderConfirmWindow
					&& IsValidEnginePath(Core::enginePath.string()))
				{
					GUI::RunProject(file);
				}
			}

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);

			if (ImGui::Button("Remove", ImVec2(200, 50)))
			{
				if (!renderConfirmWindow)
				{
					confirmFileName = fileName;
					confirmFilePath = file;
					renderConfirmWindow = true;
				}
			}

			ImGui::End();

			nextPanelPos.y += panelHeight + panelSpacing;
		}
	}

	ImGui::EndChild();

	ImGui::End();
}

void GUI::RenderButtons()
{
	glfwGetFramebufferSize(Core::window, &framebufferWidth, &framebufferHeight);

	ImGui::SetNextWindowSize(ImVec2(300, static_cast<float>(framebufferHeight)));
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));

	ImGuiWindowFlags mainWindowFlags =
		ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoSavedSettings;

	ImGui::Begin("Buttons", NULL, mainWindowFlags);

	if (ImGui::Button("New Project", ImVec2(285, 50)))
	{
		if (!renderConfirmWindow) GUI::NewProject();
	}

	ImGui::Dummy(ImVec2(0.0f, 15.0f));

	if (ImGui::Button("Add Project", ImVec2(285, 50)))
	{
		if (!renderConfirmWindow) GUI::AddProject();
	}

	ImGui::Dummy(ImVec2(0.0f, 15.0f));

	if (ImGui::Button("Set engine path", ImVec2(285, 50)))
	{
		if (!renderConfirmWindow) GUI::SetEnginePathBySelection();
	}

	ImGui::End();
}

void GUI::NewProject()
{
	string filePath = SelectWithExplorer(SelectType::new_folder);

	if (filePath.empty())
	{
		cout << "Cancelled folder selection...\n\n";
		return;
	}

	if (!is_empty(filePath))
	{
		cout << "Error: Cannot create a new project inside a folder with content inside it!\n\n";
		remove_all(filePath);
		return;
	}

	path parentPath(filePath);
	parentPath = parentPath.parent_path();
	path projectFile(filePath);
	for (const auto& entry : directory_iterator(parentPath))
	{
		path entryFile(entry);
		if (entry.is_regular_file()
			&& entryFile.stem() == projectFile.stem())
		{
			cout << "Error: A project with the name '" << projectFile.stem().string() << "' already exists in the same folder!\n\n";
			remove_all(filePath);
			return;
		}
	}

	string scenePath = filePath + "/scene.txt";
	ofstream scene(scenePath);
	if (!scene.is_open())
	{
		cout << "Error: Failed to open scene file at '" << scenePath << "'!\n\n";
		remove_all(filePath);
		return;
	}

	scene.close();

	string sceneDirectory = filePath + "/Scene1";
	create_directory(sceneDirectory);

	rename(scenePath, sceneDirectory + "/scene.txt");

	string projectFilePath = filePath + "/project.txt";
	ofstream project(projectFilePath);
	if (!project.is_open())
	{
		cout << "Error: Failed to open project file at '" << projectFilePath << "'!\n\n";
		remove_all(filePath);
		return;
	}

	string engineParentPath = Core::enginePath.parent_path().string();
	project << "scene: " << filePath + "/Scene1/scene.txt\n";
	project << "project: " << filePath + "/project.txt" << "\n";
	project << "game: \n";
	project.close();

	ofstream projectsFile(Core::projectsFilePath, ios::app);
	if (!projectsFile.is_open())
	{
		cout << "Error: Failed to open projects file!\n\n";
		return;
	}
	projectsFile << filePath << "\n";
	projectsFile.close();
	UpdateFileList();

	cout << "Successfully created new project at '" << filePath << "'!\n\n";
}

void GUI::AddProject()
{
	string filePath = SelectWithExplorer(SelectType::existing_file);

	if (filePath.empty())
	{
		cout << "Cancelled project selection...\n\n";
		return;
	}
	 
	for (const auto& element : files)
	{
		if (element == filePath)
		{
			cout << "Error: '" << filePath << "' has already been added!\n\n";
			UpdateFileList();
			return;
		}
	}

	ofstream projectsFile(Core::projectsFilePath, ios::app);
	if (!projectsFile.is_open())
	{
		cout << "Error: Failed to open projects file!\n\n";
		return;
	}
	projectsFile << filePath << "\n";
	projectsFile.close();
	UpdateFileList();

	cout << "Added existing project '" << filePath << "'!\n\n";
}

void GUI::SetEnginePathBySelection()
{
	string filePath = SelectWithExplorer(SelectType::engine_path);

	if (filePath.empty())
	{
		cout << "Cancelled engine selection...\n\n";
		return;
	}

	path enginePath(filePath);
	if (enginePath.stem().string() != "Elypso engine"
		|| enginePath.extension().string() != ".exe")
	{
		cout << "Error: Path " << filePath << " does not lead to Elypso engine.exe!\n\n";
		return;
	}

	ofstream configFile(Core::configFilePath);
	if (!configFile.is_open())
	{
		cout << "Error: Failed to open config file!\n\n";
		return;
	}
	configFile << filePath << "\n";
	configFile.close();

	Core::enginePath = filePath;

	cout << "Set engine path to '" << Core::enginePath << "'!\n\n";
}
void GUI::SetEnginePathFromConfigFile()
{
	ifstream configFile(Core::configFilePath);
	if (!configFile.is_open())
	{
		cout << "Error: Failed to open config file at '" << Core::configFilePath << "'!\n\n";
		return;
	}

	string firstLine;
	if (!getline(configFile, firstLine))
	{
		if (configFile.eof()) cout << "Error: Config file is empty!\n\n";
		else cout << "Error: Couldn't read first line from file!\n\n";
		configFile.close();
		return;
	}

	if (firstLine.empty())
	{
		cout << "Error: Config file is empty!\n\n";
		configFile.close();
		return;
	}

	if (!exists(firstLine))
	{
		cout << "Error: Engine path '" << firstLine << "' read from config file does not exist!\n\n";
		configFile.close();
		return;
	}

	path enginePath(firstLine);
	if (enginePath.stem().string() != "Elypso engine"
		|| enginePath.extension().string() != ".exe")
	{
		cout << "Error: Path '" << firstLine << "' does not lead to Elypso engine.exe!\n\n";
		configFile.close();
		return;
	}
	Core::enginePath = enginePath.string();

	configFile.close();

	cout << "Set engine path to " << Core::enginePath << "!\n\n";
}

void GUI::ConfirmRemove(const string& projectName, const string& projectPath)
{
	ImGui::SetNextWindowPos(ImVec2(400, 200));
	ImGui::SetNextWindowSize(ImVec2(500, 300));

	ImGuiWindowFlags flags = 
		ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoSavedSettings;

	string title = "Remove " + projectName + "?";
	ImGui::Begin(title.c_str(), nullptr, flags);

	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImVec2 windowCenter(windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f);
	ImVec2 buttonSize(120, 50);
	float buttonSpacing = 20.0f;

	ImVec2 buttonYesPos(
		windowSize.x * 0.45f - buttonSize.x,
		windowSize.y * 0.5f - buttonSize.y * 0.5f);
	ImVec2 buttonNoPos(
		windowSize.x * 0.55f,
		windowSize.y * 0.5f - buttonSize.y * 0.5f);
	
	ImGui::SetCursorPos(buttonYesPos);
	if (ImGui::Button("Yes", buttonSize))
	{
		GUI::RemoveProject(projectPath);
		renderConfirmWindow = false;
	}

	ImGui::SetCursorPos(buttonNoPos);
	if (ImGui::Button("No", buttonSize))
	{
		cout << "Cancelled project removal...\n\n";
		renderConfirmWindow = false;
	}

	ImGui::End();
}
void GUI::RemoveProject(const string& projectPath)
{
	if (!exists(projectPath))
	{
		cout << "Error: Tried to remove '" << projectPath << "' but it has already been removed!\n\n";
		UpdateFileList();
		return;
	}

	remove_all(projectPath);
	UpdateFileList();

	cout << "Removed '" << projectPath << "'...\n\n";
}

bool GUI::IsValidEnginePath(const string& enginePath)
{
	if (Core::enginePath == "")
	{
		SetEnginePathFromConfigFile();
	}

	if (Core::enginePath == "")
	{
		cout << "Error: Couldn't run engine because no valid path could be loaded!\n\n";
		return false;
	}

	if (!exists(Core::enginePath))
	{
		cout << "Error: Tried to run '" << Core::enginePath << "' but it doesn't exist!\n\n";
		return false;
	}

	if (Core::enginePath.stem().string() != "Elypso engine"
		|| Core::enginePath.extension().string() != ".exe")
	{
		cout << "Error: Path '" << Core::enginePath << "' does not lead to Elypso engine.exe!\n\n";
		return false;
	}

	return true;
}

void GUI::RunProject(const string& targetProject)
{
	if (!exists(targetProject))
	{
		cout << "Error: Trying to run project that no longer exists!\n";
		UpdateFileList();
		return;
	}

	//copy project.txt to files folder
	string engineFilesFolderPath = Core::enginePath.parent_path().string() + "/files";
	string originalProjectFile = targetProject + "/project.txt";
	string targetProjectFile = engineFilesFolderPath + "/project.txt";
	if (exists(targetProjectFile)) remove(targetProjectFile);
	copy(originalProjectFile, targetProjectFile);

	cout << "Running engine from '" << Core::enginePath << "'!\n\n";

	cout << ".\n.\n.\n\n";

	GUI::RunApplication(
		Core::enginePath.parent_path().string(),
		Core::enginePath.string());
}

void GUI::RunApplication(const string& parentFolderPath, const string& exePath, const string& commands)
{
	wstring wParentFolderPath(parentFolderPath.begin(), parentFolderPath.end());
	wstring wExePath(exePath.begin(), exePath.end());
	wstring wCommands(commands.begin(), commands.end());

	//initialize structures for process creation
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	// Create the new process
	if (!CreateProcessW
	(
		wExePath.c_str(),          //path to the executable
		const_cast<LPWSTR>(wCommands.c_str()), //command line arguments
		nullptr,                   //process handle not inheritable
		nullptr,                   //thread handle not inheritable
		FALSE,                     //handle inheritance
		0,                         //creation flags
		nullptr,                   //use parent's environment block
		wParentFolderPath.c_str(), //use parent's starting directory
		&si,                       //pointer to STARTUPINFO structure
		&pi                        //pointer to PROCESS_INFORMATION structure
	))
	{
		//retrieve the error code and print a descriptive error message
		LPVOID lpMsgBuf = nullptr;
		DWORD dw = GetLastError();
		FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER
			| FORMAT_MESSAGE_FROM_SYSTEM
			| FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)&lpMsgBuf, 0, nullptr);
		std::wcout << L"Error: " << reinterpret_cast<LPCWSTR>(lpMsgBuf) << L"\n\n";
		LocalFree(lpMsgBuf);
	}

	// Close process and thread handles
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

void GUI::UpdateFileList()
{
	files.clear();

	ifstream projectsFile(Core::projectsFilePath);

	if (!projectsFile.is_open())
	{
		cout << "Error: Failed to open projects file!\n\n";
		return;
	}

	string line;
	while (getline(projectsFile, line))
	{
		if (!exists(line)) foundInvalidPath = true;
		else files.push_back(line);
	}

	projectsFile.close();

	if (files.size() > 0
		&& foundInvalidPath)
	{
		ofstream editedProjectsFile(Core::projectsFilePath);
		if (!editedProjectsFile.is_open())
		{
			cout << "Error: Failed to open projects file!\n\n";
			return;
		}

		for (const auto& file : files)
		{
			editedProjectsFile << file << "\n";
		}

		editedProjectsFile.close();

		foundInvalidPath = false;
	}
}

string GUI::SelectWithExplorer(SelectType selectType)
{
	//initialize COM
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(hr))
	{
		cout << "Error: Failed to initialize COM!\n\n";
		return "";
	}

	//create an instance of the File Open dialog
	IFileOpenDialog* pFileOpen = nullptr;
	hr = CoCreateInstance(
		CLSID_FileOpenDialog, 
		NULL,
		CLSCTX_ALL,
		IID_IFileOpenDialog,
		reinterpret_cast<void**>(&pFileOpen));
	if (FAILED(hr))
	{
		cout << "Error: Failed to create File Open dialog!\n\n";
		CoUninitialize();
		return "";
	}

	if (selectType == SelectType::new_folder
		|| selectType == SelectType::existing_file)
	{
		//restrict the selection to folders only
		DWORD dwOptions;
		hr = pFileOpen->GetOptions(&dwOptions);
		if (SUCCEEDED(hr))
		{
			hr = pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);
			if (FAILED(hr))
			{
				cout << "Error: Failed to set options!\n\n";
				pFileOpen->Release();
				CoUninitialize();
				return "";
			}
		}
		else
		{
			cout << "Error: Failed to get options!\n\n";
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}
	}

	else if (selectType == SelectType::engine_path)
	{
		//restrict file selection to .exe only
		COMDLG_FILTERSPEC filterSpec[] = { { L"Executables", L"*.exe"} };
		hr = pFileOpen->SetFileTypes(1, filterSpec);
		if (FAILED(hr))
		{
			cout << "Error: Failed to set file filter!\n\n";
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}
	}

	else if (selectType == SelectType::scene_file)
	{
		//restrict file selection to .txt only
		COMDLG_FILTERSPEC filterSpec[] = { { L"Scene files", L"*.txt"} };
		hr = pFileOpen->SetFileTypes(1, filterSpec);
		if (FAILED(hr))
		{
			cout << "Error: Failed to set file filter!\n\n";
			pFileOpen->Release();
			CoUninitialize();
			return "";
		}
	}

	//show the File Open dialog
	hr = pFileOpen->Show(NULL);
	if (FAILED(hr))
	{
		cout << "Error: Failed to show dialog!\n\n";
		pFileOpen->Release();
		CoUninitialize();
		return "";
	}

	//get the result of the user's selection
	IShellItem* pItem;
	hr = pFileOpen->GetResult(&pItem);
	if (FAILED(hr))
	{
		cout << "Error: Failed to retrieve result!\n\n";
		pFileOpen->Release();
		CoUninitialize();
		return "";
	}

	//get the path pf the selected file or folder
	PWSTR filePath;
	hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
	if (FAILED(hr))
	{
		cout << "Error: Failed to retrieve path!\n\n";
		pItem->Release();
		pFileOpen->Release();
		CoUninitialize();
		return "";
	}

	//convert the wide string to a string
	wstring ws(filePath);

	//get the required buffer size
	int size_needed = WideCharToMultiByte(
		CP_UTF8,
		0,
		ws.c_str(),
		static_cast<int>(ws.length()),
		NULL,
		0,
		NULL,
		NULL);

	//convert wide string to utf-8 encoded narrow string
	string narrowPath(size_needed, 0);
	WideCharToMultiByte(
		CP_UTF8,
		0,
		ws.c_str(),
		static_cast<int>(ws.length()),
		&narrowPath[0],
		size_needed,
		NULL,
		NULL);

	//free memory allocated for filePath
	CoTaskMemFree(filePath);

	//release the shell item
	pItem->Release();

	//release the file open dialog
	pFileOpen->Release();

	//uninitialze COM
	CoUninitialize();

	return narrowPath;
}

void GUI::Shutdown()
{
	//close any remaining open ImGui windows
	for (ImGuiWindow* window : ImGui::GetCurrentContext()->Windows)
	{
		if (window->WasActive)
		{
			ImGui::CloseCurrentPopup();
		}
	}

	ImGui::StyleColorsDark();
	ImGui::GetIO().IniFilename = nullptr;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}