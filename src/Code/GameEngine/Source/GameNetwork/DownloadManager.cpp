// cl: /O1
// stlport46
// bfmealloc_alloconly
// psdk
// Portions derived from Command & Conquer Generals and Command & Conquer Generals Zero Hour, (c) Electronic Arts Inc., GPL-3.0-or-later; see LICENSE and NOTICE.
#include <winsock2.h>
#include "GameClient/GameText.h"
#include "GameNetwork/DownloadManager.h"
// Tier B; object=O005E048B evidence=Z flags=align=A;ehform=EH_prolog
// Extent is a hypothesis; only Object VERIFIED establishes recovery.
// Edges: left=unknown-rows right=unknown-rows
// The download queue manager's three trivial overrides: update forwards to the
// interface at +0x08, OnEnd raises the finished byte at +0x0D, and the progress
// callback ignores its arguments.
// FUNCTION: BFME2 0x005E048B size=7 name=?update@DownloadManager@@QAEJXZ evidence=N:ledger

HRESULT DownloadManager::update(void)
{
	return m_download->PumpMessages();
}

// FUNCTION: BFME2 0x005E0492 size=7 name=?OnEnd@DownloadManager@@UAEJXZ evidence=N:ledger

HRESULT DownloadManager::OnEnd()
{
	m_sawEnd = true;
	return S_OK;
}

// FUNCTION: BFME2 0x005E0499 size=5 name=?OnProgressUpdate@DownloadManager@@UAEJHHHH@Z evidence=N:ledger

HRESULT DownloadManager::OnProgressUpdate(int bytesread, int totalsize, int timetaken, int timeleft)
{
	return S_OK;
}

// FUNCTION: BFME2 0x005E04F5 size=225 name=?downloadFile@DownloadManager@@UAEJVAsciiString@@00000_N@Z evidence=N:ledger
HRESULT DownloadManager::downloadFile(AsciiString server, AsciiString username, AsciiString password, AsciiString file, AsciiString localfile, AsciiString regkey, Bool tryResume)
{
	return m_download->DownloadFile(server.str(), username.str(), password.str(), file.str(), localfile.str(), regkey.str(), tryResume);
}

// FUNCTION: BFME2 0x005E05D6 size=82 name=?getLastLocalFile@DownloadManager@@QAE?AVAsciiString@@XZ evidence=N:ledger
AsciiString DownloadManager::getLastLocalFile(void)
{
	char buf[256] = "";
	m_download->GetLastLocalFile(buf, 256);
	return buf;
}

// FUNCTION: BFME2 0x005E0628 size=189 name=?OnError@DownloadManager@@UAEJH@Z evidence=N:ledger
HRESULT DownloadManager::OnError(int error)
{
	m_wasError = true;
	AsciiString s = "FTP:UnknownError";
	switch (error)
	{
		case DOWNLOADEVENT_NOSUCHSERVER:
			s.set("FTP:NoSuchServer");
			break;
		case DOWNLOADEVENT_COULDNOTCONNECT:
			s.set("FTP:CouldNotConnect");
			break;
		case DOWNLOADEVENT_LOGINFAILED:
			s.set("FTP:LoginFailed");
			break;
		case DOWNLOADEVENT_NOSUCHFILE:
			s.set("FTP:NoSuchFile");
			break;
		case DOWNLOADEVENT_LOCALFILEOPENFAILED:
			s.set("FTP:LocalFileOpenFailed");
			break;
		case DOWNLOADEVENT_TCPERROR:
			s.set("FTP:TCPError");
			break;
		case DOWNLOADEVENT_DISCONNECTERROR:
			s.set("FTP:DisconnectError");
			break;
	}
	m_errorString = TheGameText->fetch(s);
	return S_OK;
}

// FUNCTION: BFME2 0x005E06E5 size=183 name=?OnStatusUpdate@DownloadManager@@UAEJH@Z evidence=N:ledger
HRESULT DownloadManager::OnStatusUpdate(int status)
{
	AsciiString s = "FTP:StatusNone";
	switch (status)
	{
		case DOWNLOADSTATUS_CONNECTING:
			s.set("FTP:StatusConnecting");
			break;
		case DOWNLOADSTATUS_LOGGINGIN:
			s.set("FTP:StatusLoggingIn");
			break;
		case DOWNLOADSTATUS_FINDINGFILE:
			s.set("FTP:StatusFindingFile");
			break;
		case DOWNLOADSTATUS_QUERYINGRESUME:
			s.set("FTP:StatusQueryingResume");
			break;
		case DOWNLOADSTATUS_DOWNLOADING:
			s.set("FTP:StatusDownloading");
			break;
		case DOWNLOADSTATUS_DISCONNECTING:
			s.set("FTP:StatusDisconnecting");
			break;
		case DOWNLOADSTATUS_FINISHING:
			s.set("FTP:StatusFinishing");
			break;
		case DOWNLOADSTATUS_DONE:
			s.set("FTP:StatusDone");
			break;
	}
	m_statusString = TheGameText->fetch(s);
	return S_OK;
}

// FUNCTION: BFME2 0x005E07C0 size=235 name=?downloadNextQueuedFile@DownloadManager@@QAEJXZ evidence=N:ledger
HRESULT DownloadManager::downloadNextQueuedFile(void)
{
	QueuedDownload q;
	_STL::list<QueuedDownload>::iterator it = m_queuedDownloads.begin();
	if (it != m_queuedDownloads.end())
	{
		q = *it;
		m_queuedDownloads.pop_front();
		m_wasError = m_sawEnd = false;
		return downloadFile(q.server, q.userName, q.password, q.file, q.localFile, q.regKey, q.tryResume);
	}
	else
	{
		return S_OK;
	}
}

// FUNCTION: BFME2 0x005E08AB size=233 name=??0DownloadManager@@QAE@XZ evidence=N:ledger
DownloadManager::DownloadManager()
{
	m_download = new CDownload(this);
	m_wasError = m_sawEnd = false;
	m_queuedDownloads.clear();
	m_statusString.set(TheGameText->fetch("FTP:StatusIdle"));

	// ----- Initialize Winsock -----
	m_winsockInit = true;
	WORD verReq = MAKEWORD(2, 2);
	WSADATA wsadata;

	int err = WSAStartup(verReq, &wsadata);
	if (err != 0)
	{
		m_winsockInit = false;
	}
	else
	{
		if ((LOBYTE(wsadata.wVersion) != 2) || (HIBYTE(wsadata.wVersion) != 2))
		{
			WSACleanup();
			m_winsockInit = false;
		}
	}
}

// FUNCTION: BFME2 0x005E0994 size=118 name=??1DownloadManager@@UAE@XZ evidence=N:ledger
DownloadManager::~DownloadManager()
{
	delete m_download;
	if (m_winsockInit)
	{
		WSACleanup();
		m_winsockInit = false;
	}
}

// FUNCTION: BFME2 0x005E0A0A size=233 name=?queueFileForDownload@DownloadManager@@QAEXVAsciiString@@00000_N@Z evidence=N:ledger

void DownloadManager::queueFileForDownload(AsciiString server, AsciiString username, AsciiString password, AsciiString file, AsciiString localfile, AsciiString regkey, Bool tryResume)
{
	QueuedDownload q;
	q.file = file;
	q.localFile = localfile;
	q.password = password;
	q.regKey = regkey;
	q.server = server;
	q.tryResume = tryResume;
	q.userName = username;
	m_queuedDownloads.push_back(q);
}
