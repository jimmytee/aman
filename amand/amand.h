// amand.h
//
// amand(8) Monitoring Daemon.
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: amand.h,v 1.17 2013/11/19 00:14:40 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifndef AMAND_H
#define AMAND_H

#define AMAND_USAGE "-d\n"
#define AMAND_PID_FILE "/var/run/amand.pid"

#include <QtCore/QObject>
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtCore/QDateTime>
#include <QtSql/QSqlDatabase>

#include <config.h>
#include <state.h>

#include "replicationtest.h"
#include "streamcmdserver.h"
#include "pingmonitor.h"

class MainObject : public QObject
{
 Q_OBJECT;
 public:
  MainObject(QObject *parent=0);
  ~MainObject();

 private slots:
  void commandReceivedData(int id,int cmd,const QStringList &args);
  void thatStateChangedData(bool ping,bool running,bool accessible,
			    State::ClusterState db_state,
			    const QString &snapshot,int replication_time,
			    State::ClusterState audio_state,
			    bool audio_status);
  void checkStatusData();
  void replicationTestCompleteData(bool success,int msecs);
  void autoRotateData();
  void startAudioCopy();
  void rsyncFinishedData(int exitcode,QProcess::ExitStatus status);
  void rsyncErrorData(QProcess::ProcessError err);

 private:
  void ScheduleAudioCopy(int msecs);
  void StopAudioCopy();
  void SetAudioStatus(bool status);
  void AutoRotate();
  void PurgeBinlogs();
  void DeleteBinlogSequence(const QString &basename,unsigned last) const;
  QStringList StateUpdateArgs() const;
  bool CheckLocalState();
  bool IsMysqlRunning() const;
  bool IsMysqlAccessible(int testval);
  void ScheduleAutoRotation();
  bool GenerateMysqlSnapshot(const QString &filename);
  bool RestoreMysqlSnapshot(const QString &filename,QString *binlog,int *pos);
  bool SetMysqlMetadata(const QString &binlog_name,int binlog_pos);
  bool StopSlaves();
  QSqlDatabase Db();
  bool OpenMysql(Am::Instance inst,Config::Address addr);
  void CloseMysql();
  bool PushFile(const QString &srcfile,const QString &desthost,
		const QString &destfile);
  QString MakeTempDir() const;
  QString MakeSnapshotName() const;
  void SendAlert(const QString &msg);
  bool debug;
  StreamCmdServer *main_cmd_server;
  PingMonitor *main_monitor;
  ReplicationTest *main_repl_test;
  Config *main_config;
  QDir *main_snapshot_dir;
  QTimer *main_auto_rotate_timer;
  bool main_auto_rotate_state;
  State *main_state;
  bool main_replication_test_state;
  QProcess *main_audio_process;
  QTimer *main_audio_holdoff_timer;
};


#endif  // AMAND_H
