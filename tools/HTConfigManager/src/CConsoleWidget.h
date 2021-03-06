/*
 * Copyright (C) 2013 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This file is part of Holoren.
 *
 * Holoren is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Holoren is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Holoren.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 */

#ifndef CCONSOLEWIDGET_H
#define CCONSOLEWIDGET_H

#include <QTextEdit>
#include <QProcess>
#include <QString>


/**
 * A class that creates the actual console widget
 */
class CConsoleWidget : public QTextEdit
{
    Q_OBJECT
  public:
    /**
     * Default constructor
     *
     * @param start_script the code that has to be executed at the very begining
     *                     of the console run
     */
    explicit CConsoleWidget(const QString & start_script = QString(),
                            const QString & hist_file = QString(),
                            QWidget *parent = 0);

    /**
     * A method to load the command history
     */
    bool loadHistory(const QString & hist_file);

    /**
     * A method to save the command history
     */
    bool saveHistory(const QString & hist_file);

  signals:
    void finished(void);
    void error(QString error);

  protected:
    virtual void keyPressEvent(QKeyEvent *event);

  private slots:
    void handleStdoutRead(void);
    void handleStderrRead(void);
    void handleProcessError(QProcess::ProcessError);
    void handleProcessFinished(int exitCode, QProcess::ExitStatus);
    void handleCursorPositionChanged(void);

  private:
    /**
     * A method that will extract the user's current command
     * from the console
     */
    QString getUserCommand(void) const;

    /**
     * A method to remove the substring from QtextEdit that
     * represents the user's command for shell
     */
    void replaceUserCommand(const QString & new_cmd);

    /**
     * Send the user command to the underlying shell
     */
    void writeToShellProc(QString data);

  private:
    int m_fixed_position;    /// determines the fixed position on the line, beyond which the cursor cannot go
    int m_history_selected;  /// currently selected command from history
    QStringList m_history;   /// history of commands
    QProcess m_shell_proc;   /// the shell process that will be interpreting the console commands
};

#endif // CCONSOLEWIDGET_H

