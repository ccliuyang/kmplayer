/***************************************************************************
                          kmplayerconfig.cpp  -  description
                             -------------------
    begin                : 2002/12/30
    copyright            : (C) 2002 by Koos Vriezen
    email                : |EMAIL|
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>

#include <qcheckbox.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qmultilineedit.h>
#include <qtabwidget.h>
#include <qslider.h>

#include <kconfig.h>
#include <kfiledialog.h>
#include <kapplication.h>
#include <kurl.h>
#include <kdebug.h>

#include "kmplayerconfig.h"
#include "kmplayer_part.h"
#include "kmplayerview.h"
//#include "configdialog.h"
#include "pref.h"

TVChannel::TVChannel (const QString & n, int f) : name (n), frequency (f) {}

TVInput::TVInput (const QString & n, int _id) : name (n), id (_id) {
    channels.setAutoDelete (true);
}

TVDevice::TVDevice (const QString & d, const QSize & s) : device (d), size (s) {
    inputs.setAutoDelete (true);
}

KMPlayerConfig::KMPlayerConfig (KMPlayer * player, KConfig * config)
  : configdialog (0L), m_config (config), m_player (player) {
}

KMPlayerConfig::~KMPlayerConfig () {
    // configdialog should be destroyed when the view is destroyed
    //delete configdialog;
}

static const char * strMPlayerGroup = "MPlayer";
static const char * strGeneralGroup = "General Options";
static const char * strMPlayerPatternGroup = "MPlayer Output Matching";
static const char * strKeepSizeRatio = "Keep Size Ratio";
//static const char * strUseArts = "Use aRts";
static const char * strVoDriver = "Video Driver";
static const char * strAoDriver = "Audio Driver";
static const char * strAddArgs = "Additional Arguments";
static const char * strSize = "Movie Size";
static const char * strCache = "Cache Fill";
static const char * strIndexPattern = "Index Pattern";
static const char * strStart = "Start Playing";
static const char * strShowConsole = "Show Console Output";
static const char * strLoop = "Loop";
static const char * strShowControlButtons = "Show Control Buttons";
static const char * strShowPositionSlider = "Show Position Slider";
static const char * strAddConfigButton = "Add Configure Button";
static const char * strAutoHideButtons = "Auto Hide Control Buttons";
//static const char * strAutoHideSlider = "Auto Hide Slider"; 
static const char * strSeekTime = "Forward/Backward Seek Time";
static const char * strCacheSize = "Cache Size for Streaming";
static const char * strPlayDVD = "Immediately Play DVD";
static const char * strShowDVD = "Show DVD Menu";
static const char * strDVDDevice = "DVD Device";
static const char * strLanguagePattern = "DVD Language";
static const char * strSubtitlePattern = "DVD Sub Title";
static const char * strTitlePattern = "DVD Titles";
static const char * strChapterPattern = "DVD Chapters";
static const char * strPlayVCD = "Immediately Play VCD";
static const char * strShowVCD = "Show VCD Menu";
static const char * strVCDDevice = "VCD Device";
static const char * strTrackPattern = "VCD Tracks";
static const char * strAlwaysBuildIndex = "Always build index";
// postproc thingies
static const char * strPPGroup = "Post processing options";
static const char * strPostProcessing = "Post processing";
static const char * strDisablePPauto = "Automaticly disable post processing";
static const char * strPP_Default = "Default preset";
static const char * strPP_Fast = "Fast preset";
static const char * strPP_Custom = "Custom preset";

static const char * strCustom_Hz = "Horizontal deblocking";
static const char * strCustom_Hz_Aq = "Horizontal deblocking auto quality";
static const char * strCustom_Hz_Ch = "Horizontal deblocking chrominance";

static const char * strCustom_Vt = "Vertical deblocking";
static const char * strCustom_Vt_Aq = "Vertical deblocking auto quality";
static const char * strCustom_Vt_Ch = "Vertical deblocking chrominance";

static const char * strCustom_Dr = "Dering filter";
static const char * strCustom_Dr_Aq = "Dering auto quality";
static const char * strCustom_Dr_Ch = "Dering chrominance";

static const char * strCustom_Al = "Autolevel";
static const char * strCustom_Al_F = "Autolevel full range";

static const char * strCustom_Tn = "Temporal Noise Reducer";
static const char * strCustom_Tn_S = "Temporal Noise Reducer strength";

static const char * strPP_Lin_Blend_Int = "Linear Blend Deinterlacer";
static const char * strPP_Lin_Int = "Linear Interpolating Deinterlacer";
static const char * strPP_Cub_Int = "Cubic Interpolating Deinterlacer";
static const char * strPP_Med_Int = "Median Interpolating Deinterlacer";
static const char * strPP_FFmpeg_Int = "FFmpeg Interpolating Deinterlacer";
// end of postproc

void KMPlayerConfig::readConfig () {
    KMPlayerView *view = static_cast <KMPlayerView *> (m_player->view ());

    m_config->setGroup (strGeneralGroup);
    //usearts = m_config->readBoolEntry (strUseArts, true);
    

    m_config->setGroup (strMPlayerGroup);
    sizeratio = m_config->readBoolEntry (strKeepSizeRatio, true);
    view->setKeepSizeRatio (sizeratio);
    showconsole = m_config->readBoolEntry (strShowConsole, false);
    view->setShowConsoleOutput (showconsole);
    loop = m_config->readBoolEntry (strLoop, false);
    showbuttons = m_config->readBoolEntry (strShowControlButtons, true);
    autohidebuttons = m_config->readBoolEntry (strAutoHideButtons, false);
    view->setAutoHideButtons (showbuttons && autohidebuttons);
    if (!showbuttons) {
        view->buttonBar ()->hide ();
    }
    showposslider = m_config->readBoolEntry(strShowPositionSlider, true);
    if (!showposslider)
        view->positionSlider ()->hide ();
    else
        view->positionSlider ()->show ();
    //autohideslider = m_config->readBoolEntry(strAutoHideSlider, true);
    showcnfbutton = m_config->readBoolEntry (strAddConfigButton, true);
    if (showcnfbutton)
        view->configButton ()->show ();
    else
        view->configButton ()->hide ();
    seektime = m_config->readNumEntry (strSeekTime, 10);
    m_player->setSeekTime (seektime);
    alwaysbuildindex = m_config->readBoolEntry (strAlwaysBuildIndex, false);
    playdvd = m_config->readBoolEntry (strPlayDVD, true);
    dvddevice = m_config->readEntry (strDVDDevice, "/dev/dvd");
    //showdvdmenu = m_config->readBoolEntry (strShowDVD, true);
    playvcd = m_config->readBoolEntry (strPlayVCD, true);
    vcddevice = m_config->readEntry (strVCDDevice, "/dev/cdrom");
    //showvcdmenu = m_config->readBoolEntry (strShowVCD, true);
    videodriver = m_config->readNumEntry (strVoDriver, VDRIVER_XV_INDEX);
    audiodriver = m_config->readNumEntry (strAoDriver, ADRIVER_DEFAULT_INDEX);
    if (audiodriver == ADRIVER_ARTS_INDEX)
    	view->setUseArts (true);
    else
    	view->setUseArts (false);
    additionalarguments = m_config->readEntry (strAddArgs, "");
    cachesize = m_config->readNumEntry (strCacheSize, 0);
    m_player->setCacheSize (cachesize);
    m_config->setGroup (strMPlayerPatternGroup);
    sizepattern = m_config->readEntry (strSize, "VO:.*[^0-9]([0-9]+)x([0-9]+)");
    cachepattern = m_config->readEntry (strCache, "Cache fill:[^0-9]*([0-9\\.]+)%");
    indexpattern = m_config->readEntry (strIndexPattern, "Generating Index: +([0-9]+)%");
    startpattern = m_config->readEntry (strStart, "Start[^ ]* play");
    langpattern = m_config->readEntry (strLanguagePattern, "\\[open].*audio.*language: ([A-Za-z]+).*aid.*[^0-9]([0-9]+)");
    subtitlespattern = m_config->readEntry (strSubtitlePattern, "\\[open].*subtitle.*[^0-9]([0-9]+).*language: ([A-Za-z]+)");
    titlespattern = m_config->readEntry (strTitlePattern, "There are ([0-9]+) titles");
    chapterspattern = m_config->readEntry (strChapterPattern, "There are ([0-9]+) chapters");
    trackspattern = m_config->readEntry (strTrackPattern, "track ([0-9]+):");

    
    // postproc
    m_config->setGroup (strPPGroup);
    postprocessing = m_config->readBoolEntry (strPostProcessing, false);
    disableppauto = m_config->readBoolEntry (strDisablePPauto, true);
    
    pp_default = m_config->readBoolEntry (strPP_Default, true);
    pp_fast = m_config->readBoolEntry (strPP_Fast, false);
    pp_custom = m_config->readBoolEntry (strPP_Custom, false);
    // default these to default preset
    pp_custom_hz = m_config->readBoolEntry (strCustom_Hz, true);
    pp_custom_hz_aq = m_config->readBoolEntry (strCustom_Hz_Aq, true);
    pp_custom_hz_ch = m_config->readBoolEntry (strCustom_Hz_Ch, false);

    pp_custom_vt = m_config->readBoolEntry (strCustom_Vt, true);
    pp_custom_vt_aq = m_config->readBoolEntry (strCustom_Vt_Aq, true);
    pp_custom_vt_ch = m_config->readBoolEntry (strCustom_Vt_Ch, false);

    pp_custom_dr = m_config->readBoolEntry (strCustom_Dr, true);
    pp_custom_dr_aq = m_config->readBoolEntry (strCustom_Dr_Aq, true);
    pp_custom_dr_ch = m_config->readBoolEntry (strCustom_Dr_Ch, false);

    pp_custom_al = m_config->readBoolEntry (strCustom_Al, true);
    pp_custom_al_f = m_config->readBoolEntry (strCustom_Al_F, false);

    pp_custom_tn = m_config->readBoolEntry (strCustom_Tn, true);
    pp_custom_tn_s = m_config->readNumEntry (strCustom_Tn_S, 0);

    pp_lin_blend_int = m_config->readBoolEntry (strPP_Lin_Blend_Int, false);
    pp_lin_int = m_config->readBoolEntry (strPP_Lin_Int, false);
    pp_cub_int = m_config->readBoolEntry (strPP_Cub_Int, false);
    pp_med_int = m_config->readBoolEntry (strPP_Med_Int, false);
    pp_ffmpeg_int = m_config->readBoolEntry (strPP_FFmpeg_Int, false);

    // TV stuff
    tvdevices.clear ();
    m_config->setGroup("TV");
    tvdriver = m_config->readEntry ("Driver", "v4l");
    QStrList devlist;
    int deviceentries = m_config->readListEntry ("Devices", devlist, ';');
    for (int i = 0; i < deviceentries; i++) {
        m_config->setGroup (devlist.at (i));
        TVDevice * device = new TVDevice (devlist.at (i), 
                                          m_config->readSizeEntry ("Size"));
        QStrList inputlist;
        int inputentries = m_config->readListEntry ("Inputs", inputlist, ';');
        kdDebug() << device->device << " has " << inputentries << " inputs" << endl;
        for (int j = 0; j < inputentries; j++) {
            QString inputstr = inputlist.at (j);
            int pos = inputstr.find (':');
            if (pos < 0) {
                kdError () << "Wrong input: " << inputstr << endl;
                continue;
            }
            TVInput * input = new TVInput (inputstr.mid (pos + 1),
                                           inputstr.left (pos).toInt ());
            QStrList freqlist;
            int freqentries = m_config->readListEntry(input->name,freqlist,';');
            kdDebug() << input->name<< " has " << freqentries << " freqs" << endl;
            input->hastuner = (freqentries > 0);
            for (int k = 0; k < freqentries; k++) {
                QString freqstr = freqlist.at (k);
                int pos = freqstr.find (':');
                if (pos < 0) {
                    kdError () << "Wrong frequency: " << freqstr << endl;
                    continue;
                }
                TVChannel * channel = new TVChannel (freqstr.left (pos),
                                                  freqstr.mid (pos+1).toInt ());
                kdDebug() << freqstr.left (pos) << " at " << freqstr.mid (pos+1).toInt() << endl;
                input->channels.append (channel);
            }
            device->inputs.append (input);
        }
        tvdevices.append (device);
    }
}

void KMPlayerConfig::show () {
    if (!configdialog) {
        configdialog = new KMPlayerPreferences (m_player->view ());
        connect (configdialog, SIGNAL (okClicked ()), 
                this, SLOT (okPressed ()));
        connect (configdialog, SIGNAL (applyClicked ()), 
                this, SLOT (okPressed ()));
        /*connect (configdialog->openFile, SIGNAL (clicked ()), 
                this, SLOT (fileOpen ()));
        connect (configdialog->showDVDMenu, SIGNAL (toggled (bool)), 
                configdialog->dvdTab, SLOT (setEnabled (bool)));
        connect (configdialog->showVCDMenu, SIGNAL (toggled (bool)), 
                configdialog->vcdTab, SLOT (setEnabled (bool)));
        connect (configdialog->showControlButtons, SIGNAL (toggled (bool)), 
                configdialog->addConfigButton, SLOT (setEnabled (bool)));
        connect (configdialog->showControlButtons, SIGNAL (toggled (bool)), 
                configdialog->autoHideControlButtons, SLOT (setEnabled (bool)));
        connect (configdialog->haveVideoDriver, SIGNAL (toggled (bool)), 
                configdialog->videoDriver, SLOT (setEnabled (bool)));
        connect (configdialog->haveCache, SIGNAL (toggled (bool)), 
                configdialog->cacheSize, SLOT (setEnabled (bool)));
        connect (configdialog->haveArguments, SIGNAL (toggled (bool)), 
                configdialog->additionalArguments, SLOT (setEnabled (bool)));*/
        if (KApplication::kApplication())
            connect (configdialog, SIGNAL (helpClicked ()),
                     this, SLOT (getHelp ()));
        /*else
            configdialog->buttonHelp->hide ();*/
    }
    //configdialog->url->setText (m_player->url ().url ());
    configdialog->m_GeneralPageGeneral->keepSizeRatio->setChecked (sizeratio);
    //configdialog->useArts->setChecked (usearts); //replaced in the dialog
    configdialog->m_GeneralPageGeneral->showConsoleOutput->setChecked (showconsole); //works
    configdialog->m_GeneralPageGeneral->loop->setChecked (loop); //works
    configdialog->m_GeneralPageGeneral->showControlButtons->setChecked (showbuttons); //works
    configdialog->m_GeneralPageGeneral->showPositionSlider->setChecked (showposslider);
    configdialog->m_GeneralPageGeneral->alwaysBuildIndex->setChecked (alwaysbuildindex);
    //configdialog->m_GeneralPageGeneral->autoHideSlider->setChecked (autohideslider);
    //configdialog->addConfigButton->setChecked (showcnfbutton);	//not
    //configdialog->addConfigButton->setEnabled (showbuttons);		//yet
    configdialog->m_GeneralPageGeneral->autoHideControlButtons->setChecked (autohidebuttons); //works
    //configdialog->m_GeneralPageGeneral->autoHideControlButtons->setEnabled (showbuttons);
    configdialog->m_GeneralPageGeneral->seekTime->setValue(seektime);
    configdialog->m_GeneralPageDVD->autoPlayDVD->setChecked (playdvd); //works if autoplay?
    //configdialog->showDVDMenu->setChecked (showdvdmenu);
    //configdialog->dvdTab->setEnabled (showdvdmenu);
    configdialog->m_GeneralPageDVD->dvdDevicePath->setText (dvddevice);
    configdialog->m_GeneralPageVCD->autoPlayVCD->setChecked (playvcd);
    //configdialog->showVCDMenu->setChecked (showvcdmenu);
    //configdialog->vcdTab->setEnabled (showvcdmenu);
    configdialog->m_GeneralPageVCD->vcdDevicePath->setText (vcddevice);
    //configdialog->haveVideoDriver->setChecked (videodriver.length () > 0);
    //configdialog->videoDriver->setEnabled (videodriver.length () > 0);
    
    configdialog->m_GeneralPageOutput->videoDriver->setCurrentItem (videodriver);
    configdialog->m_GeneralPageOutput->audioDriver->setCurrentItem (audiodriver);
    
    
    //configdialog->haveCache->setChecked (cachesize > 0);
    //configdialog->cacheSize->setEnabled (cachesize > 0);
    if (cachesize > 0)
        configdialog->m_GeneralPageAdvanced->cacheSize->setValue(cachesize);
    //bool haveArgs = additionalarguments.length () > 0;
    //configdialog->haveArguments->setChecked (haveArgs);
    /*configdialog->additionalArguments->setEnabled (haveArgs);
    if (haveArgs)*/
    configdialog->m_GeneralPageAdvanced->additionalArguments->setText (additionalarguments);
    configdialog->m_GeneralPageAdvanced->sizePattern->setText (sizepattern);
    configdialog->m_GeneralPageAdvanced->cachePattern->setText (cachepattern);
    configdialog->m_GeneralPageAdvanced->startPattern->setText (startpattern);
    configdialog->m_GeneralPageAdvanced->indexPattern->setText (indexpattern);
    configdialog->m_GeneralPageAdvanced->dvdLangPattern->setText (langpattern);
    configdialog->m_GeneralPageAdvanced->dvdSubPattern->setText (subtitlespattern);
    configdialog->m_GeneralPageAdvanced->dvdTitlePattern->setText (titlespattern);
    configdialog->m_GeneralPageAdvanced->dvdChapPattern->setText (chapterspattern);
    configdialog->m_GeneralPageAdvanced->vcdTrackPattern->setText (trackspattern);
    
    // postproc
    configdialog->m_OPPagePostproc->postProcessing->setChecked (postprocessing);
    configdialog->m_OPPagePostproc->postProcessing->setChecked (disableppauto);
    if (postprocessing)
            configdialog->m_OPPagePostproc->PostprocessingOptions->setEnabled (true);
    
    configdialog->m_OPPagePostproc->defaultPreset->setChecked (pp_default);
    configdialog->m_OPPagePostproc->fastPreset->setChecked (pp_fast);
    configdialog->m_OPPagePostproc->customPreset->setChecked (pp_custom);

    configdialog->m_OPPagePostproc->HzDeblockFilter->setChecked (pp_custom_hz);
    configdialog->m_OPPagePostproc->HzDeblockAQuality->setChecked (pp_custom_hz_aq);
    configdialog->m_OPPagePostproc->HzDeblockCFiltering->setChecked (pp_custom_hz_ch);

    configdialog->m_OPPagePostproc->VtDeblockFilter->setChecked (pp_custom_vt);
    configdialog->m_OPPagePostproc->VtDeblockAQuality->setChecked (pp_custom_vt_aq);
    configdialog->m_OPPagePostproc->VtDeblockCFiltering->setChecked (pp_custom_vt_ch);

    configdialog->m_OPPagePostproc->DeringFilter->setChecked (pp_custom_dr);
    configdialog->m_OPPagePostproc->DeringAQuality->setChecked (pp_custom_dr_aq);
    configdialog->m_OPPagePostproc->DeringCFiltering->setChecked (pp_custom_dr_ch);

    configdialog->m_OPPagePostproc->AutolevelsFilter->setChecked (pp_custom_al);
    configdialog->m_OPPagePostproc->AutolevelsFullrange->setChecked (pp_custom_al_f);
    configdialog->m_OPPagePostproc->TmpNoiseFilter->setChecked (pp_custom_tn);
    //configdialog->m_OPPagePostproc->TmpNoiseSlider->setValue (pp_custom_tn_s);

    configdialog->m_OPPagePostproc->LinBlendDeinterlacer->setChecked (pp_lin_blend_int);
    configdialog->m_OPPagePostproc->LinIntDeinterlacer->setChecked (pp_lin_int);
    configdialog->m_OPPagePostproc->CubicIntDeinterlacer->setChecked (pp_cub_int);
    configdialog->m_OPPagePostproc->MedianDeinterlacer->setChecked (pp_med_int);
    configdialog->m_OPPagePostproc->FfmpegDeinterlacer->setChecked (pp_ffmpeg_int);

  
    configdialog->show ();
}

void KMPlayerConfig::writeConfig () {
    KMPlayerView *view = static_cast <KMPlayerView *> (m_player->view ());
    
    m_config->setGroup (strGeneralGroup);
    //m_config->writeEntry (strUseArts, usearts);
    m_config->setGroup (strMPlayerGroup);
    m_config->writeEntry (strKeepSizeRatio, view->keepSizeRatio ());
    m_config->writeEntry (strShowConsole, view->showConsoleOutput());
    m_config->writeEntry (strLoop, loop);
    m_config->writeEntry (strSeekTime, m_player->seekTime ());
    m_config->writeEntry (strVoDriver, videodriver);
    m_config->writeEntry (strAoDriver, audiodriver);
    m_config->writeEntry (strAddArgs, additionalarguments);
    m_config->writeEntry (strCacheSize, m_player->cacheSize ());
    m_config->writeEntry (strShowControlButtons, showbuttons);
    m_config->writeEntry (strShowPositionSlider, showposslider);
    m_config->writeEntry (strAlwaysBuildIndex, alwaysbuildindex);
    //m_config->writeEntry (strAutoHideSlider, autohideslider);
    m_config->writeEntry (strAddConfigButton, showcnfbutton);
    m_config->writeEntry (strAutoHideButtons, autohidebuttons);
    m_config->writeEntry (strPlayDVD, playdvd);
    
    m_config->writeEntry (strDVDDevice, dvddevice);
    m_config->writeEntry (strPlayVCD, playvcd);
    
    m_config->writeEntry (strVCDDevice, vcddevice);
    m_config->setGroup (strMPlayerPatternGroup);
    m_config->writeEntry (strSize, sizepattern);
    m_config->writeEntry (strCache, cachepattern);
    m_config->writeEntry (strIndexPattern, indexpattern);
    m_config->writeEntry (strStart, startpattern);
    m_config->writeEntry (strLanguagePattern, langpattern);
    m_config->writeEntry (strSubtitlePattern, subtitlespattern);
    m_config->writeEntry (strTitlePattern, titlespattern);
    m_config->writeEntry (strChapterPattern, chapterspattern);
    m_config->writeEntry (strTrackPattern, trackspattern);
    //postprocessing stuff
    m_config->setGroup (strPPGroup);
    m_config->writeEntry (strPostProcessing, postprocessing);
    m_config->writeEntry (strDisablePPauto, disableppauto);
    m_config->writeEntry (strPP_Default, pp_default);
    m_config->writeEntry (strPP_Fast, pp_fast);
    m_config->writeEntry (strPP_Custom, pp_custom);

    m_config->writeEntry (strCustom_Hz, pp_custom_hz);
    m_config->writeEntry (strCustom_Hz_Aq, pp_custom_hz_aq);
    m_config->writeEntry (strCustom_Hz_Ch, pp_custom_hz_ch);

    m_config->writeEntry (strCustom_Vt, pp_custom_vt);
    m_config->writeEntry (strCustom_Vt_Aq, pp_custom_vt_aq);
    m_config->writeEntry (strCustom_Vt_Ch, pp_custom_vt_ch);

    m_config->writeEntry (strCustom_Dr, pp_custom_dr);
    m_config->writeEntry (strCustom_Dr_Aq, pp_custom_vt_aq);
    m_config->writeEntry (strCustom_Dr_Ch, pp_custom_vt_ch);

    m_config->writeEntry (strCustom_Al, pp_custom_al);
    m_config->writeEntry (strCustom_Al_F, pp_custom_al_f);

    m_config->writeEntry (strCustom_Tn, pp_custom_tn);
    m_config->writeEntry (strCustom_Tn_S, pp_custom_tn_s);

    m_config->writeEntry (strPP_Lin_Blend_Int, pp_lin_blend_int);
    m_config->writeEntry (strPP_Lin_Int, pp_lin_int);
    m_config->writeEntry (strPP_Cub_Int, pp_cub_int);
    m_config->writeEntry (strPP_Med_Int, pp_med_int);
    m_config->writeEntry (strPP_FFmpeg_Int, pp_ffmpeg_int);

    m_config->sync ();
}

void KMPlayerConfig::okPressed () {
    KMPlayerView *view = static_cast <KMPlayerView *> (m_player->view ());
    if (!view)
        return;
    /*bool urlchanged = m_player->url () != KURL (configdialog->url->text ());
    if (m_player->url ().isEmpty () && configdialog->url->text ().isEmpty ())
        urlchanged = false; // hmmm aren't these URLs the same?

    if (urlchanged)
        m_player->setURL (configdialog->url->text ());*/
    
    sizeratio = configdialog->m_GeneralPageGeneral->keepSizeRatio->isChecked ();
    m_player->keepMovieAspect (sizeratio);
    //usearts = configdialog->useArts->isChecked ();
    //view->setUseArts (false); //audio driver selection TO BE implemented in another way
    showconsole = configdialog->m_GeneralPageGeneral->showConsoleOutput->isChecked ();
    view->setShowConsoleOutput (showconsole);
    alwaysbuildindex = configdialog->m_GeneralPageGeneral->alwaysBuildIndex->isChecked();
    loop = configdialog->m_GeneralPageGeneral->loop->isChecked ();
    if (showconsole && !m_player->playing ())
        view->consoleOutput ()->show ();
    else
        view->consoleOutput ()->hide ();
    showbuttons = configdialog->m_GeneralPageGeneral->showControlButtons->isChecked ();
    if (showbuttons)
        view->buttonBar ()->show ();
    else
        view->buttonBar ()->hide ();
    autohidebuttons = configdialog->m_GeneralPageGeneral->autoHideControlButtons->isChecked ();
    view->setAutoHideButtons (autohidebuttons);
    showposslider = configdialog->m_GeneralPageGeneral->showPositionSlider->isChecked ();
    if (!showposslider)
        view->positionSlider ()->hide ();
    else
        view->positionSlider ()->show ();
    //autohideslider = configdialog->m_GeneralPageGeneral->autoHideSlider->isChecked();
    //view->setAutoHideSlider(autohideslider);
    //showcnfbutton = configdialog->m_GeneralPageGeneral->addConfigButton->isChecked ();
    showcnfbutton = true;
    if (showcnfbutton)
	view->configButton ()->show ();
    else
        view->configButton ()->hide ();
    playdvd = configdialog->m_GeneralPageDVD->autoPlayDVD->isChecked ();
    dvddevice = configdialog->m_GeneralPageDVD->dvdDevicePath->text ();
    playvcd = configdialog->m_GeneralPageVCD->autoPlayVCD->isChecked ();
    vcddevice = configdialog->m_GeneralPageVCD->vcdDevicePath->text ();
    seektime = configdialog->m_GeneralPageGeneral->seekTime->value();
    m_player->setSeekTime (seektime); 
    
    additionalarguments = configdialog->m_GeneralPageAdvanced->additionalArguments->text();
    cachesize = configdialog->m_GeneralPageAdvanced->cacheSize->value(); 
    m_player->setCacheSize (cachesize);
    sizepattern = configdialog->m_GeneralPageAdvanced->sizePattern->text ();
    cachepattern = configdialog->m_GeneralPageAdvanced->cachePattern->text ();
    startpattern = configdialog->m_GeneralPageAdvanced->startPattern->text ();
    indexpattern = configdialog->m_GeneralPageAdvanced->indexPattern->text ();
    langpattern = configdialog->m_GeneralPageAdvanced->dvdLangPattern->text ();
    titlespattern = configdialog->m_GeneralPageAdvanced->dvdTitlePattern->text ();
    subtitlespattern = configdialog->m_GeneralPageAdvanced->dvdSubPattern->text ();
    chapterspattern = configdialog->m_GeneralPageAdvanced->dvdChapPattern->text ();
    trackspattern = configdialog->m_GeneralPageAdvanced->vcdTrackPattern->text ();
    
    videodriver = configdialog->m_GeneralPageOutput->videoDriver->currentItem();
    audiodriver = configdialog->m_GeneralPageOutput->audioDriver->currentItem();
    if (audiodriver == ADRIVER_ARTS_INDEX)
    	    view->setUseArts(true);
    //postproc
    postprocessing = configdialog->m_OPPagePostproc->postProcessing->isChecked();
    disableppauto = configdialog->m_OPPagePostproc->disablePPauto->isChecked();
    pp_default = configdialog->m_OPPagePostproc->defaultPreset->isChecked();
    pp_fast = configdialog->m_OPPagePostproc->fastPreset->isChecked();
    pp_custom = configdialog->m_OPPagePostproc->customPreset->isChecked();

    pp_custom_hz = configdialog->m_OPPagePostproc->HzDeblockFilter->isChecked();
    pp_custom_hz_aq = configdialog->m_OPPagePostproc->HzDeblockAQuality->isChecked();
    pp_custom_hz_ch = configdialog->m_OPPagePostproc->HzDeblockCFiltering->isChecked();

    pp_custom_vt = configdialog->m_OPPagePostproc->VtDeblockFilter->isChecked();
    pp_custom_vt_aq = configdialog->m_OPPagePostproc->VtDeblockAQuality->isChecked();
    pp_custom_vt_ch = configdialog->m_OPPagePostproc->VtDeblockCFiltering->isChecked();

    pp_custom_dr = configdialog->m_OPPagePostproc->DeringFilter->isChecked();
    pp_custom_dr_aq = configdialog->m_OPPagePostproc->DeringAQuality->isChecked();
    pp_custom_dr_ch = configdialog->m_OPPagePostproc->DeringCFiltering->isChecked();

    pp_custom_al = configdialog->m_OPPagePostproc->AutolevelsFilter->isChecked();
    pp_custom_al_f = configdialog->m_OPPagePostproc->AutolevelsFullrange->isChecked();

    pp_custom_tn = configdialog->m_OPPagePostproc->TmpNoiseFilter->isChecked();
    pp_custom_tn_s = 0; // gotta fix this later
    //pp_custom_tn_s = configdialog->m_OPPagePostproc->TmpNoiseSlider->value();

    pp_lin_blend_int = configdialog->m_OPPagePostproc->LinBlendDeinterlacer->isChecked();
    pp_lin_int = configdialog->m_OPPagePostproc->LinIntDeinterlacer->isChecked();
    pp_cub_int = configdialog->m_OPPagePostproc->CubicIntDeinterlacer->isChecked();
    pp_med_int = configdialog->m_OPPagePostproc->MedianDeinterlacer->isChecked();
    pp_ffmpeg_int = configdialog->m_OPPagePostproc->FfmpegDeinterlacer->isChecked();
    
    writeConfig ();

    emit configChanged ();

    /*if (urlchanged) {
        m_player->stop ();
        if (m_player->browserextension ())
            m_player->browserextension ()->urlChanged (m_player->url ().url ());
        
        m_player->play ();
    }*/
	//m_player->openURL (KURL (configdialog->url->text ()));

    }



void KMPlayerConfig::fileOpen () {
    /*KFileDialog *dlg = new KFileDialog (QString::null, QString::null, configdialog, "", true);
    if (dlg->exec ())
        configdialog->url->setText (dlg->selectedURL().url ());
    delete dlg;*/
}

void KMPlayerConfig::getHelp () {
    KApplication::kApplication()->invokeBrowser ("man:/mplayer");
}

#include "kmplayerconfig.moc"

