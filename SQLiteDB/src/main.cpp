#include <windows.h>
#include <tchar.h>
#include <sqlite3.h>
#include "Database.h"
#include "TableSchemas.h"
#include <iostream>
#include <cstdio>
#include <io.h>
#include <fcntl.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' \
version='6.0.0.0' \
processorArchitecture='*' \
publicKeyToken='6595b64144ccf1df' \
language='*'\"")

static TCHAR szWindowClass[] = _T("DesktopApp");
static TCHAR szTitle[] = _T("Test App");
HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
Database* db = nullptr;

// Global handles for the input box and button
#define IDC_TEXTBOX_INPUT_NAME   101
#define IDC_TEXTBOX_INPUT_EMAIL  102
#define IDC_TEXTBOX_INPUT_SCORE  103
#define IDC_TEXTBOX_INPUT_DELETE 104
#define IDC_BUTTON_SAVE          105
#define IDC_BUTTON_DELETE		 106
#define IDC_USER_LISTBOX		 107

// make brushes for all the controls
HWND hNameInputBox =   NULL;
HBRUSH hBackgroundBrush = NULL;

HWND hEmailInputBox =  NULL;
HBRUSH hButtonBrush = NULL;

HWND hScoreInputBox =  NULL;
HBRUSH hListboxBrush = NULL;

HWND hDeleteInputBox = NULL;
HBRUSH hDeleteBrush = NULL;


HWND hButtonSave =	   NULL;
HBRUSH hButtonSaveBrush = NULL;

HWND hButtonDelete =   NULL;
HBRUSH hButtonDeleteBrush = NULL;

HWND hUserListbox =    NULL;
HBRUSH hUserListboxBrush = NULL;


// Redirect C and C++ standard streams to the newly allocated console
static void RedirectIOToConsole()
{
	FILE* fp = nullptr;
	// Reopen stdin/out/err to the console
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	// Ensure CRT file descriptors are in text mode
	_setmode(_fileno(stdout), _O_TEXT);
	_setmode(_fileno(stderr), _O_TEXT);
	_setmode(_fileno(stdin), _O_TEXT);

	// Sync C++ streams with C stdio and clear any error state
	std::ios::sync_with_stdio(true);
	std::cout.clear();
	std::cerr.clear();
	std::cin.clear();
}

void RefreshUserListbox(HWND hListbox, Database* db) {
	// 1. Wipe out everything currently visible in the Listbox
	SendMessage(hListbox, LB_RESETCONTENT, 0, 0);

	// 2. Fetch the fresh array/vector from your database
	auto allUsers = db->getAllUserData();

	for (size_t i = 0; i < allUsers.size(); ++i) {
		// Safe reference to the specific row
		const auto& user = allUsers[i];

		// Format data with your separator
		std::wstring userInfo =
			std::wstring(user[0].begin(), user[0].end()) + L" / " +
			std::wstring(user[1].begin(), user[1].end()) + L" / " +
			std::wstring(user[2].begin(), user[2].end()) + L" / " +
			std::wstring(user[3].begin(), user[3].end());

		// Add the text row to the listbox
		int index = SendMessage(hListbox, LB_ADDSTRING, 0, (LPARAM)userInfo.c_str());

		// 3. FIXED BUG: Convert the text string ID safely to an integer 
		// and save it directly inside the item data slot instead of a dynamic pointer.
		int rawUserID = _ttoi(std::wstring(user[0].begin(), user[0].end()).c_str());
		SendMessage(hListbox, LB_SETITEMDATA, index, (LPARAM)rawUserID);
	}
}

int WINAPI _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow) {

// Show CMD for debugging purposes
#if defined(_DEBUG)
	if (AllocConsole()) {
		RedirectIOToConsole();
	}
#endif

	{	// SQLITE FUNCTIONS START HERE
		
		std::cout << "Starting the database application..." << std::endl;

		// Open the Database
		db = new Database();
		if (!db->open("test.db")) {
			return 1;
		}



		// Create the Users Table
		std::string createTableSQL =
			"CREATE TABLE IF NOT EXISTS users ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT,"
			"name TEXT NOT NULL,"
			"email TEXT NOT NULL UNIQUE,"
			"score INTEGER DEFAULT 0"
			");";



		// Verify Table Creation and Output Result
		std::cout << "Creating the users table..." << std::endl;
		if (db->execute(createTableSQL)) {
			std::cout << "Users table created successfully." << std::endl;
		}
		else {
			std::cerr << "Failed to create users table." << std::endl;
		}

		// Insert a Single Instance of Test Data into the Users Table
		std::string insertDataSQL =
			"INSERT INTO users (name, email, score) VALUES "
			"('Tobi', 'Tobi@example.com', 20),"
			"('Martin', 'Martin@example.com', 30),"
			"('Ingrid', 'Ingrid@example.com', 0);";

		// Verify Data Insertion and Output Result
		std::cout << "Inserting test data into the users table..." << std::endl;
		if (db->execute(insertDataSQL)) {
			std::cout << "Test data inserted successfully." << std::endl;
		}
		else {
			std::cerr << "Failed to insert test data." << std::endl;
		}

		std::cout << "Program ran successfully." << std::endl;
		std::cout << "Closing the database..." << std::endl;

		db->printAllUsers();
	}

/* Note: _In_, _In_opt_ are SAL annotations defined in Windows headers */
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hbrBackground = CreateSolidBrush(RGB(15, 15, 20));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}



	hInst = hInstance; // Store instance handle in our global variable	

	HWND hWnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		1920, 1080,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindowEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}




LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR greeting[] = _T("Hello, Windows desktop!");
	TCHAR greeting2[] = _T("Alz is gay");

	auto data = db->getUserDataByID(3);
	std::wstring DataConverted = std::wstring(data.begin(), data.end());
	const wchar_t* Score = DataConverted.c_str();

	switch (message) {

		// NEW CASE: Fires ONCE when your window opens
	case WM_CREATE: {
		// Create the Text Input Box (EDIT Control)
		hNameInputBox = CreateWindowEx(0, _T("EDIT"), _T(""),
			WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			5, 100, 200, 25, hWnd, (HMENU)IDC_TEXTBOX_INPUT_NAME, NULL, NULL);
		hBackgroundBrush = CreateSolidBrush(RGB(5, 5, 15));
		SendMessage(hNameInputBox, EM_SETCUEBANNER, FALSE, (LPARAM)L"Name...");

		hEmailInputBox = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""),
			WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			230, 100, 200, 25, hWnd, (HMENU)IDC_TEXTBOX_INPUT_EMAIL, NULL, NULL);
		SendMessage(hEmailInputBox, EM_SETCUEBANNER, FALSE, (LPARAM)L"Email...");

		hScoreInputBox = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""),
			WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			455, 100, 200, 25, hWnd, (HMENU)IDC_TEXTBOX_INPUT_SCORE, NULL, NULL);
		SendMessage(hScoreInputBox, EM_SETCUEBANNER, FALSE, (LPARAM)L"Score...");

		hDeleteInputBox = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""),
			WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			700, 100, 200, 25, hWnd, (HMENU)IDC_TEXTBOX_INPUT_DELETE, NULL, NULL);
		SendMessage(hDeleteInputBox, EM_SETCUEBANNER, FALSE, (LPARAM)L"Select User ID...");

		// Create the Clickable Action Button (BUTTON Control)
		hButtonSave = CreateWindowEx(NULL, _T("BUTTON"), _T("Add User"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			5, 135, 100, 30, hWnd, (HMENU)IDC_BUTTON_SAVE, NULL, NULL);
		hButtonDelete = CreateWindowEx(NULL, _T("BUTTON"), _T("Delete User"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			750, 135, 100, 30, hWnd, (HMENU)IDC_BUTTON_DELETE, NULL, NULL);

		// Create the User List Box (LISTBOX Control)
		hUserListbox = CreateWindowEx(WS_EX_CLIENTEDGE, _T("LISTBOX"), _T(""),
			WS_CHILD | WS_VISIBLE | LBS_NOTIFY,
			5, 170, 600, 300, hWnd, (HMENU)IDC_USER_LISTBOX, NULL, NULL);

		// Populate the List Box with User Data from the Database
		RefreshUserListbox(hUserListbox, db);

		break;
	}

				  // NEW CASE: Listens for when buttons are clicked
	case WM_COMMAND: {
		// Check if our specific button was clicked
		if (LOWORD(wParam) == IDC_BUTTON_SAVE) {
			TCHAR textBufferName[256];
			TCHAR textBufferEmail[256];
			TCHAR textBufferScore[256];

			// Extract whatever text the user typed into the input field
			GetWindowText(hNameInputBox, textBufferName, 256);
			GetWindowText(hEmailInputBox, textBufferEmail, 256);
			GetWindowText(hScoreInputBox, textBufferScore, 256);

			// Now you can convert 'textBuffer' to a string and send it to your SQLite database!
			// std::wstring enteredText(textBuffer);
			// db->execute(...);

			// Forces the window to visually refresh its text fields
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (LOWORD(wParam) == IDC_BUTTON_DELETE) {
			TCHAR textBufferID[256];
			GetWindowText(hDeleteInputBox, textBufferID, 256);

			db->deleteUser(_ttoi(textBufferID));

			RefreshUserListbox(hUserListbox, db);
		}
		break;
	}

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);


		TextOut(hdc,
			5, 5,
			greeting, static_cast<int>(_tcslen(greeting)));

		TextOut(hdc,
			5, 30,
			greeting2, static_cast<int>(_tcslen(greeting2)));

		TextOut(hdc,
			5, 60,
			Score, static_cast<int>(_tcslen(Score)));


		EndPaint(hWnd, &ps);
		break;

	case WM_CTLCOLOREDIT:
	{
		HDC hdcEdit = (HDC)wParam;        // Handle to the input box canvas
		HWND hwndEdit = (HWND)lParam;     // Handle to the input box control window

		// Check if Windows is drawing your specific name input box
		if (hwndEdit == hNameInputBox)
		{
			SetTextColor(hdcEdit, RGB(255, 255, 255)); // Set your custom text color
			SetBkColor(hdcEdit, RGB(5, 5, 15));       // Match the text background to the brush

			return (INT_PTR)hBackgroundBrush;        // Return the solid brush to paint the box background
		}

		// Allow other edit boxes to look normal
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	case WM_DESTROY:
		// Clean up GDI objects to prevent memory leaks
		if (hBackgroundBrush)
		{
			DeleteObject(hBackgroundBrush);
		}

		// Close the database connection and free memory
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

