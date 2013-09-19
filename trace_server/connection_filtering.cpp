void MainWindow::onRegexAdd ()
{
	Connection * conn = m_server->findCurrentConnection();
	if (!conn) return;

	QString qItem = ui->comboBoxRegex->currentText();

	conn->onRegexAdd(qItem);
/*
 *
	if (!qItem.length())
		return;
	QStandardItem * root = static_cast<QStandardItemModel *>(getWidgetRegex()->model())->invisibleRootItem();
	QStandardItem * child = findChildByText(root, qItem);
	if (child == 0)
	{
		QList<QStandardItem *> row_items = addTriRow(qItem, Qt::Unchecked, true);
		root->appendRow(row_items);
		conn->appendToRegexFilters(qItem, false, true);
		conn->recompileRegexps();
	}
	*/
}

void MainWindow::onRegexRm ()
{
	/*QStandardItemModel * model = static_cast<QStandardItemModel *>(getWidgetRegex()->model());
	QModelIndex const idx = getWidgetRegex()->currentIndex();
	QStandardItem * item = model->itemFromIndex(idx);
	if (!item)
		return;
	QString const & val = model->data(idx, Qt::DisplayRole).toString();
	model->removeRow(idx.row());*/
	Connection * conn = m_server->findCurrentConnection();
	if (conn)
	{
		conn->onRegexRm();
		//conn->removeFromRegexFilters(val);
		//conn->recompileRegexps();
	}
}

void MainWindow::onStringAdd ()
{
	Connection * conn = m_server->findCurrentConnection();
	if (!conn) return;

	QString qItem = ui->qFilterLineEdit->text();
	conn->onStringAdd(qItem);

	/*
	 *
	 *
	if (!qItem.length())
		return;
	 * QStandardItem * root = static_cast<QStandardItemModel *>(getWidgetString()->model())->invisibleRootItem();
	QStandardItem * child = findChildByText(root, qItem);
	if (child == 0)
	{
		QList<QStandardItem *> row_items = addTriRow(qItem, Qt::Checked, true);
		root->appendRow(row_items);
		conn->appendToStringFilters(qItem, true, true);
		row_items[0]->setCheckState(Qt::Checked);
		conn->recompileStrings();
	}*/
}

void MainWindow::onStringRm ()
{
	/*QStandardItemModel * model = static_cast<QStandardItemModel *>(getWidgetString()->model());
	QModelIndex const idx = getWidgetString()->currentIndex();
	QStandardItem * item = model->itemFromIndex(idx);
	if (!item)
		return;
	QString const & val = model->data(idx, Qt::DisplayRole).toString();
	model->removeRow(idx.row());*/
	Connection * conn = m_server->findCurrentConnection();
	if (conn)
	{
		conn->onStringRm();
		//conn->removeFromStringFilters(val);
		//conn->recompileStrings();
	}
}

// @TODO: all the color stuff is almost duplicate, remove duplicity
void MainWindow::onColorRegexActivate (int idx)
{
	if (idx == -1) return;
	if (!getTabTrace()->currentWidget()) return;

	onColorRegexAdd();
}

void MainWindow::onColorRegexAdd ()
{
	Connection * conn = m_server->findCurrentConnection();
	if (!conn) return;

	QString qItem = ui->comboBoxColorRegex->currentText();
	if (!qItem.length())
		return;
	QStandardItem * root = static_cast<QStandardItemModel *>(getWidgetColorRegex()->model())->invisibleRootItem();
	QStandardItem * child = findChildByText(root, qItem);
	if (child == 0)
	{
		QList<QStandardItem *> row_items = addRow(qItem, false);
		root->appendRow(row_items);

		conn->appendToColorRegexFilters(qItem);
	}
	conn->recompileColorRegexps();
}

void MainWindow::onColorRegexRm ()
{
	Connection * conn = m_server->findCurrentConnection();
	QStandardItemModel * model = static_cast<QStandardItemModel *>(getWidgetColorRegex()->model());
	QModelIndex const idx = getWidgetColorRegex()->currentIndex();
	QStandardItem * item = model->itemFromIndex(idx);
	if (!item)
		return;
	QString const & val = model->data(idx, Qt::DisplayRole).toString();
	model->removeRow(idx.row());

	if (conn)
	{
		conn->removeFromColorRegexFilters(val);
		conn->recompileColorRegexps();
	}
}


void Server::onClearCurrentView ()
{
	if (Connection * conn = findCurrentConnection())
		conn->onClearCurrentView();
}
void Server::onClearCurrentFileFilter ()
{
	if (Connection * conn = findCurrentConnection())
		conn->onClearCurrentFileFilter();
}
void Server::onClearCurrentCtxFilter ()
{
	if (Connection * conn = findCurrentConnection())
		conn->onClearCurrentCtxFilter();
}
void Server::onClearCurrentTIDFilter ()
{
	if (Connection * conn = findCurrentConnection())
		conn->onClearCurrentTIDFilter();
}
void Server::onClearCurrentRegexFilter ()
{
	if (Connection * conn = findCurrentConnection())
		conn->onClearCurrentRegexFilter();
}
void Server::onClearCurrentStringFilter ()
{
	if (Connection * conn = findCurrentConnection())
		conn->onClearCurrentStringFilter();
}
void Server::onClearCurrentColorizedRegexFilter ()
{
	if (Connection * conn = findCurrentConnection())
		conn->onClearCurrentColorizedRegexFilter();
}
void Server::onClearCurrentScopeFilter ()
{
	if (Connection * conn = findCurrentConnection())
		conn->onClearCurrentScopeFilter();
}
void Server::onClearCurrentRefTime ()
{
	if (Connection * conn = findCurrentConnection())
		conn->onClearCurrentRefTime();
}

void Server::onHidePrevFromRow ()
{
	if (Connection * conn = findCurrentConnection())
		conn->onHidePrevFromRow();
}

void Server::onUnhidePrevFromRow ()
{
	if (Connection * conn = findCurrentConnection())
		conn->onUnhidePrevFromRow();
}

void Server::onExcludeFileLine ()
{
	if (Connection * conn = findCurrentConnection())
		conn->onExcludeFileLine();
}

void Server::onToggleRefFromRow ()
{
	if (Connection * conn = findCurrentConnection())
		conn->onToggleRefFromRow();
}

void Server::onApplyColumnSetup ()
{
	qDebug("%s", __FUNCTION__);
	foreach (connections_t::value_type item, m_connections)
	{
		item.second->onApplyColumnSetup();
	}
}




void Server::onClickedAtColorRegexList (QModelIndex idx)
{
	if (!idx.isValid()) return;
	QStandardItemModel * model = static_cast<QStandardItemModel *>(m_main_window->getWidgetColorRegex()->model());
	QStandardItem * item = model->itemFromIndex(idx);
	Q_ASSERT(item);

	QString const & val = model->data(idx, Qt::DisplayRole).toString();
	bool const orig_checked = (item->checkState() == Qt::Checked);
	Qt::CheckState const checked = orig_checked ? Qt::Unchecked : Qt::Checked;
	qDebug("color regex click! (checked=%u) %s ", checked, val.toStdString().c_str());
	item->setCheckState(checked);
	if (Connection * conn = findCurrentConnection())
	{
		// @TODO: if state really changed
		conn->m_session_state.setColorRegexChecked(val, checked);
		conn->recompileColorRegexps();
		conn->onInvalidateFilter();
	}
}



