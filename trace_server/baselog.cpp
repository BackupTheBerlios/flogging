#include "baselog.h"
//#include <QScrollBar>
//#include <QSplitter>
#include "connection.h"
#include "utils.h"
#include "utils_qstandarditem.h"
//#include "delegates.h"
#include "syncwidgets.h"


namespace logs {

	BaseLog::BaseLog (Connection * oparent, QWidget * wparent, LogConfig & cfg, QString const & fname)
		: QFrame(wparent)
		, m_config(cfg)
		, m_config_ui(cfg, this)
		, m_fname(fname)
		, m_connection(oparent)
	{
		//qDebug("%s this=0x%08x", __FUNCTION__, this);

		setContextMenuPolicy(Qt::CustomContextMenu);
		connect(this, SIGNAL(customContextMenuRequested(QPoint const &)), this, SLOT(onShowContextMenu(QPoint const &)));

		setConfigValuesToUI(m_config);
		onApplyButton();
		//setUpdatesEnabled(true);
		//horizontalHeader()->setSectionsMovable(true);
		//setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

		connect(&getSyncWidgets(), SIGNAL( requestTimeSynchronization(int, unsigned long long, void *) ),
							 this, SLOT( performTimeSynchronization(int, unsigned long long, void *) ));
		connect(this, SIGNAL( requestTimeSynchronization(int, unsigned long long, void *) ),
							 &getSyncWidgets(), SLOT( performTimeSynchronization(int, unsigned long long, void *) ));
		connect(&getSyncWidgets(), SIGNAL( requestFrameSynchronization(int, unsigned long long, void *) ),
							 this, SLOT( performFrameSynchronization(int, unsigned long long, void *) ));
		connect(this, SIGNAL( requestFrameSynchronization(int, unsigned long long, void *) ),
							 &getSyncWidgets(), SLOT( performFrameSynchronization(int, unsigned long long, void *) ));
	}

	BaseLog::~BaseLog ()
	{
		//qDebug("%s this=0x%08x", __FUNCTION__, this);
		for (logviews_t::iterator it = m_logviews.begin(), ite = m_logviews.end(); it != ite; ++it)
		{
			LogView * logview = *it;
			//logview->m_logview->detach();
			delete logview;
		}
		m_logviews.clear();
		disconnect(this, SIGNAL(customContextMenuRequested(QPoint const &)), this, SLOT(onShowContextMenu(QPoint const &)));
	}

	void BaseLog::onShow ()
	{
		show();
	}

	void BaseLog::onHide ()
	{
		hide();
	}

	void BaseLog::onHideContextMenu ()
	{
		Ui::SettingsLog * ui = m_config_ui.ui();
		disconnect(ui->applyButton, SIGNAL(clicked()), this, SLOT(onApplyButton()));
		disconnect(ui->saveButton, SIGNAL(clicked()), this, SLOT(onSaveButton()));
		m_config_ui.onHideContextMenu();
	}

	void BaseLog::onShowContextMenu (QPoint const & pos)
	{
		//qDebug("%s this=0x%08x", __FUNCTION__, this);
		m_config_ui.onShowContextMenu(QCursor::pos());
		Ui::SettingsLog * ui = m_config_ui.ui();

		setConfigValuesToUI(m_config);
		connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(onApplyButton()));
		connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(onSaveButton()));
		//connect(ui->logViewComboBox, SIGNAL(activated(int)), this, SLOT(onLogViewActivate(int)));
	}

	void BaseLog::applyConfig (LogConfig & cfg)
	{
		//qDebug("%s this=0x%08x", __FUNCTION__, this);
		Ui::SettingsLog * ui = m_config_ui.ui();

		for (size_t i = 0, ie = m_config.m_gvcfg.size(); i < ie; ++i)
		{
			LogViewConfig & gvc = cfg.m_gvcfg[i];

			//gvc.setTimeUnits(gvc.m_strtimeunits);
		}
	}

	void BaseLog::setConfigValuesToUI (LogConfig const & cfg)
	{
		//qDebug("%s this=0x%08x", __FUNCTION__, this);
		Ui::SettingsLog * ui = m_config_ui.ui();
		
		ui->globalShowCheckBox->blockSignals(true);
		ui->globalShowCheckBox->setCheckState(cfg.m_show ? Qt::Checked : Qt::Unchecked);
		ui->globalShowCheckBox->blockSignals(false);

		ui->logViewComboBox->clear();
		for (size_t i = 0, ie = cfg.m_gvcfg.size(); i < ie; ++i)
		{
			LogViewConfig const & gvcfg = cfg.m_gvcfg[i];
			ui->logViewComboBox->addItem(gvcfg.m_tag);
		}
		if (cfg.m_gvcfg.size())
			setViewConfigValuesToUI(cfg.m_gvcfg[0]);
	}

	void BaseLog::setUIValuesToConfig (LogConfig & cfg)
	{
		//qDebug("%s this=0x%08x", __FUNCTION__, this);
		Ui::SettingsLog * ui = m_config_ui.ui();
		m_config.m_show = ui->globalShowCheckBox->checkState() == Qt::Checked;

		for (size_t i = 0, ie = cfg.m_gvcfg.size(); i < ie; ++i)
		{
			LogViewConfig & gvcfg = cfg.m_gvcfg[i];
			setUIValuesToViewConfig(gvcfg);
		}
	}

	void BaseLog::onApplyButton ()
	{
		setUIValuesToConfig(m_config);
		applyConfig(m_config);
	}

	void BaseLog::onSaveButton ()
	{
		/*m_config.m_hsize.clear();
		m_config.m_hsize.resize(m_modelView->columnCount());
		for (int i = 0, ie = m_modelView->columnCount(); i < ie; ++i)
			m_config.m_hsize[i] = horizontalHeader()->sectionSize(i);*/
		m_connection->saveConfigForLog(m_config, m_config.m_tag);

	}
	void BaseLog::onResetButton () { setConfigValuesToUI(m_config); }
	void BaseLog::onDefaultButton ()
	{
		LogConfig defaults;
		//defaults.partialLoadFrom(m_config);
		setConfigValuesToUI(defaults);
	}

	void BaseLog::onClearAllDataButton ()
	{
	}

	void BaseLog::onClearLogViewDataButton ()
	{
	}

	void BaseLog::performTimeSynchronization (int sync_group, unsigned long long time, void * source)
	{
		qDebug("%s syncgrp=%i time=%i", __FUNCTION__, sync_group, time);
	}

	void BaseLog::performFrameSynchronization (int sync_group, unsigned long long frame, void * source)
	{
		qDebug("%s syncgrp=%i frame=%i", __FUNCTION__, sync_group, frame);
		for (logviews_t::iterator it = m_logviews.begin(), ite = m_logviews.end(); it != ite; ++it)
		{
			LogView * logview = *it;
			logview->gotoFrame(frame);
		}
	}

}


