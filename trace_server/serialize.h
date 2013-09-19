#pragma once
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include "utils.h"

template <class T>
bool loadConfigTemplate (T & t, QString const & fname)
{
	try {
		std::ifstream ifs(fname.toLatin1());
		if (!ifs) return false;
		boost::archive::xml_iarchive ia(ifs);
		ia >> BOOST_SERIALIZATION_NVP(t);
		ifs.close();
		return true;
	}
	catch (std::out_of_range const & e)
	{
		QMessageBox::critical(0, QString(__FUNCTION__), QString("OOR exception during load: %1").arg(e.what()), QMessageBox::Ok, QMessageBox::Ok);	
	}
	catch (std::length_error const & e)
	{
		QMessageBox::critical(0, QString(__FUNCTION__), QString("LE exception during load: %1").arg(e.what()), QMessageBox::Ok, QMessageBox::Ok);
	}
	catch (std::exception const & e)
	{
		QMessageBox::critical(0, QString(__FUNCTION__), QString("generic exception during load: %1").arg(e.what()), QMessageBox::Ok, QMessageBox::Ok);
	}
	catch (...)
	{
		QMessageBox::critical(0, QString(__FUNCTION__), QString("... exception during load"), QMessageBox::Ok, QMessageBox::Ok);
	}
	return false;
}

template <class T>
bool saveConfigTemplate (T const & t, QString const & fname)
{
	try {
		QFileInfo fi(fname);
		QString pname = fi.path();
		mkDir(pname);
		std::ofstream ofs(fname.toLatin1());
		if (!ofs) return false;
		boost::archive::xml_oarchive oa(ofs);
		oa << BOOST_SERIALIZATION_NVP(t);
		ofs.close();
		return true;
	}
	catch (std::out_of_range const & e)
	{
		QMessageBox::critical(0, QString(__FUNCTION__), QString("OOR exception during save: %1").arg(e.what()), QMessageBox::Ok, QMessageBox::Ok);	
	}
	catch (std::length_error const & e)
	{
		QMessageBox::critical(0, QString(__FUNCTION__), QString("LE exception during save: %1").arg(e.what()), QMessageBox::Ok, QMessageBox::Ok);
	}
	catch (std::exception const & e)
	{
		QMessageBox::critical(0, QString(__FUNCTION__), QString("generic exception during save: %1").arg(e.what()), QMessageBox::Ok, QMessageBox::Ok);
	}
	catch (...)
	{
		QMessageBox::critical(0, QString(__FUNCTION__), QString("... exception during save"), QMessageBox::Ok, QMessageBox::Ok);
	}
	return false;
}
