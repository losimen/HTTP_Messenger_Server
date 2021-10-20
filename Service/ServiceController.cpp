#include "ServiceController.h"
template <class T>
void logFile(T text);

namespace fs = std::filesystem;

#define DFILEPATH "D:\\programming\\Soft\\SoftProject\\WindowsServiceApplication\\out.txt"
extern std::ofstream dFile;

template <class T>
void logFile(T text)
{
	dFile.open(DFILEPATH, std::ios_base::app);

	dFile << text << '\n';
	dFile.close();
}


bool ServiceController::InstallService()
{
	
	if (!m_OpenSCManager(m_hSCManager))
	{
		return 0;
	}
	m_ExePath = fs::current_path().string();
	m_ExePath += '\\' + m_ExeName;
	m_hService = CreateService(
		m_hSCManager,
		get_LPCSTR_ServiceName(),
		get_LPCSTR_ServiceName(),
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		(LPCSTR)m_ExePath.c_str(),
		NULL, NULL, NULL, NULL, NULL
	);

	if (!m_hService)
	{
		int err = GetLastError();
		switch (err) {
		case ERROR_ACCESS_DENIED:
			logFile ("Error: ERROR_ACCESS_DENIED");
			break;
		case ERROR_CIRCULAR_DEPENDENCY:
			logFile("Error: ERROR_CIRCULAR_DEPENDENCY");
			break;
		case ERROR_DUPLICATE_SERVICE_NAME:
			logFile("Error: ERROR_DUPLICATE_SERVICE_NAME");
			break;
		case ERROR_INVALID_HANDLE:
			logFile("Error: ERROR_INVALID_HANDLE");
			break;
		case ERROR_INVALID_NAME:
			logFile("Error: ERROR_INVALID_NAME");
			break;
		case ERROR_INVALID_PARAMETER:
			logFile("Error: ERROR_INVALID_PARAMETER");
			break;
		case ERROR_INVALID_SERVICE_ACCOUNT:
			logFile("Error: ERROR_INVALID_SERVICE_ACCOUNT");
			break;
		case ERROR_SERVICE_EXISTS:
			logFile("Error: ERROR_SERVICE_EXISTS");
			break;
		default:
			logFile("Error: Undefined");
		}
		CloseServiceHandle(m_hSCManager);
		return 0;
	}
	CloseServiceHandle(m_hService);

	CloseServiceHandle(m_hSCManager);
	logFile("Success install service!");
	return 1;
}

bool ServiceController::UninstallService()
{
	if (!m_OpenSCManager(m_hSCManager))
	{
		return 0;
	}
	m_hService = OpenService(m_hSCManager, get_LPCSTR_ServiceName(), SERVICE_STOP | DELETE);
	if (!m_hService) {
		logFile("Error: Can't remove service");
		CloseServiceHandle(m_hSCManager);
		return 0;
	}
	
	DeleteService(m_hService);
	CloseServiceHandle(m_hService);
	CloseServiceHandle(m_hSCManager);
	logFile("Success remove service!");
	return 1;
}


bool ServiceController::StartService_()
{
	if (!m_OpenSCManager(m_hSCManager))
	{
		return 0;
	}
	m_hService = OpenService(m_hSCManager, get_LPCSTR_ServiceName(), SERVICE_START);
	if (!StartService(m_hService, 0, NULL)) {
		CloseServiceHandle(m_hSCManager);
		logFile("Error: Can't start service");
		return 0;
	}

	CloseServiceHandle(m_hService);
	CloseServiceHandle(m_hSCManager);
	return 1;
}

bool ServiceController::StopService()
{
	if (!m_OpenSCManager(m_hSCManager))
	{
		return 0;
	}
	m_hService = OpenService(m_hSCManager, get_LPCSTR_ServiceName(), SERVICE_STOP);
	SERVICE_STATUS ss;
	if (!ControlService(m_hService, SERVICE_CONTROL_STOP, &ss)) {
		logFile("Error: Can't stop service");
		logFile(GetLastError());
		CloseServiceHandle(m_hSCManager);
		return 0;
	}	

	DeleteService(m_hService);
	CloseServiceHandle(m_hService);
	CloseServiceHandle(m_hSCManager);
	logFile("Success stop service!");
	return 1;
}


bool ServiceController::m_OpenSCManager(SC_HANDLE& m_hSCManager)
{
	m_hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!m_hSCManager) {
		logFile("Error: Can't open Service Control Manager");
		return 0;
	}
	return 1;
}

LPCSTR ServiceController::get_LPCSTR_ServiceName()
{
	return (LPCSTR)m_ServiceName.c_str();
}

void ServiceController::set_ExecutableName(const char* new_name)
{
	m_ExeName = new_name;
}

void ServiceController::set_ServiceName(std::string new_name)
{
	m_ServiceName = new_name;
}
