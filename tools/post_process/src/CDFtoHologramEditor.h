/**
 */

#ifndef CDFTOHOLOGRAMEDITOR_H
#define CDFTOHOLOGRAMEDITOR_H

#include <QWidget>
#include <QString>
#include <QProcess>


class CPathPicker;
class QPushButton;
class QLabel;
class QGroupBox;
class QSpinBox;
class QDoubleSpinBox;
class QLineEdit;
class QCheckBox;
class QRadioButton;
class QXmlStreamReader;
class QTextEdit;

/**
 */
class CDFtoHologramEditor : public QWidget
{
  Q_OBJECT

  public:
    /**
     * Constructor
     *
     * @param config_file the name nad path to the config file that should be displayed and edited
     */
    explicit CDFtoHologramEditor(const QString & config_file, QWidget *parent = 0);

    /**
     * Set the path to DFtoHologram binary
     */
    void setDFtoHologramPath(const QString & path);

    /**
     */
    void setDFtoHologramWorkingDir(const QString & dir);

  signals:
    void error(QString error);
    void utilityFinished(void);

  private slots:
    /**
     * A method to run the utility associated with the given config file
     */
    bool run(void);

    /**
     */
    void handleStdoutRead(void);

    /**
     */
    void handleProcessError(QProcess::ProcessError err);

    /**
     */
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

    /**
     * A method to load settings from a given file
     */
    bool reload(void);

    /**
     * A method to save the settings
     */
    bool save(void);

  private:
    QGroupBox *createSystemInfo(const QString & config_file = QString());
    QGroupBox *createGroupEntries(void);
    QGroupBox *createGroupInterferenceBipolar(void);
    QGroupBox *createGroupBinarizeHalftonize(void);
    QLayout *createButtons(void);

  private:
    QProcess m_DFtoHologram_proc;     /// a process of DFtoHologram utility
    QLabel *m_caption;                /// editors caption

    CPathPicker *m_cfg_file;           /// the name and location of config that is being edited
    CPathPicker *m_binary_file;        /// a path to utility that uses this config
    CPathPicker *m_working_dir;        /// a working directory of the DFtoHologram utility
    QCheckBox *m_show_output;          /// whether to show the output image generated by utility

    QPushButton *m_pb_run;             /// run the corresponding utility
    QPushButton *m_pb_reload;          /// load config file
    QPushButton *m_pb_save;            /// save config file

    QTextEdit *m_DFtoHologram_output;  /// the stdout and stderr of DFtoHologram

    /* widgets to alter config entries */
    QSpinBox *m_entry_ProcessLineCnt;
    QDoubleSpinBox *m_entry_KsiXDeg;
    QDoubleSpinBox *m_entry_KsiYDeg;
    QLineEdit *m_entry_Input;
    QLineEdit *m_entry_Target;
    QCheckBox *m_entry_UsePhase;
    QRadioButton *m_entry_HologramInterference;
    QRadioButton *m_entry_HologramBipolar;
    QSpinBox *m_entry_PsDPI;
    QSpinBox *m_entry_PsDotSize;
    //QRadioButton *m_entry_Binarize;
    //QRadioButton *m_entry_Halftonize;
    QCheckBox *m_entry_Binarize;
    QCheckBox *m_entry_Halftonize;
    // mozno pridat tretiu moznost, nieco ako `None`
    QSpinBox *m_entry_HalftonizeCellSize;
    QSpinBox *m_entry_HalftonizeLevelCount;
    QCheckBox *m_entry_HalftonizeLevelByPixel;
};

#endif
