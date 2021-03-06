#ifdef WIN32
#	define _WINSOCKAPI_
#endif
#include <QApplication>
#include <QClipboard>
#include <QtGui>
#include <QMessageBox>
#include <QStatusBar>
#include <QtNetwork/QtNetwork>
#include <stdlib.h>
#include "server.h"
#include "connection.h"
#include "mainwindow.h"
#include "logs/logtablemodel.h"
#include "utils.h"
#include "tableview.h"

Server::Server (QString ip, unsigned short port, QObject * parent, bool quit_delay)
	: QTcpServer(parent)
	, m_main_window(0)
	, m_server_status("ok")
{
	m_main_window = static_cast<MainWindow *>(parent);
	QHostAddress addr(ip);
	if (!listen(addr, port)) {
		m_server_status = tr("Unable to start server! Reason: %1").arg(errorString());
		if (quit_delay)
		{
			QMessageBox::critical(0, tr("ee"), m_server_status, QMessageBox::Ok, QMessageBox::Ok);
			QTimer::singleShot(0, qApp, SLOT(quit()));
		}
		else
		{
			printf("Another instance is already running!\n");
			exit(0);
		}
		return;
	}
	m_server_status = tr("Server running at %1 port: %2").arg(serverAddress().toString()).arg(serverPort());
}

void Server::incomingConnection (qintptr socketDescriptor)
{
	Connection * connection = m_main_window->createNewConnection();
	connection->setSocketDescriptor(socketDescriptor);

	QObject::connect(connection->m_tcpstream, SIGNAL(readyRead()), connection, SLOT(processReadyRead()));
	QObject::connect(connection->m_tcpstream, SIGNAL(disconnected()), connection, SLOT(onDisconnected()));
	emit newConnection(connection);
	
	// this is supposed to use blocking reads in own thread
	/*Connection * connection = createNewTableView ();
	connection->setSocketDescriptor(socketDescriptor);
    connect(connection, SIGNAL(finished()), connection, SLOT(deleteLater()));
	connection->start();*/
}

// @FIXME: hmmm... iterates over ALL connections, that is not the intent
/*void Server::onShowPlots ()
{
	for (connections_t::iterator it = m_connections.begin(), ite = m_connections.end(); it != ite; ++it)
		it->second->onShowPlots();
}
void Server::onHidePlots ()
{
	for (connections_t::iterator it = m_connections.begin(), ite = m_connections.end(); it != ite; ++it)
		it->second->onHidePlots();
}
void Server::onShowTables ()
{
	for (connections_t::iterator it = m_connections.begin(), ite = m_connections.end(); it != ite; ++it)
		it->second->onShowTables();
}
void Server::onHideTables ()
{
	for (connections_t::iterator it = m_connections.begin(), ite = m_connections.end(); it != ite; ++it)
		it->second->onHideTables();
}*/


