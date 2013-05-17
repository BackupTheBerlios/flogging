#pragma once
#include <QWidget>
#include "config.h"
#include "tableview.h"
#include "logconfig.h"
#include "logctxmenu.h"

class Connection;

namespace logs {

	class BaseLog : public TableView
	{
		Q_OBJECT
	public:
		BaseLog (Connection * oparent, QWidget * wparent, LogConfig & cfg, QString const & fname);

		void applyConfig (LogConfig & pcfg);
		virtual ~BaseLog ();

		LogConfig & getConfig () { return m_config; }
		LogConfig const & getConfig () const { return m_config; }

	protected:

	public Q_SLOTS:

		void onShow ();
		void onHide ();
		void onHideContextMenu ();
		void onShowContextMenu (QPoint const & pos);
		void setConfigValuesToUI (LogConfig const & cfg);
		void setUIValuesToConfig (LogConfig & cfg);
		void onSaveButton ();
		void onApplyButton ();
		void onResetButton ();
		void onDefaultButton ();
		//void scrollTo (QModelIndex const & index, ScrollHint hint);
		
		//void performTimeSynchronization (int sync_group, unsigned long long time, void * source);
		//void performFrameSynchronization (int sync_group, unsigned long long frame, void * source);

		void onClearAllDataButton ();

	signals:
		//void requestTimeSynchronization (int sync_group, unsigned long long time, void * source);
		//void requestFrameSynchronization (int sync_group, unsigned long long frame, void * source);

	protected:
		LogConfig & m_config;
		logs::CtxLogConfig m_config_ui;
		QString m_fname;
		Connection * m_connection;
	};
}

