/* This file is part of the KDE project
 *
 * Copyright (C) 2003 Joonas Koivunen <rzei@mbnet.fi>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "pref.h"
#include <iostream>
#include <klocale.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qstringlist.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qgroupbox.h>
#include <qtooltip.h>
#include <qtabwidget.h>
#include <qslider.h>
#include <qbuttongroup.h>
#include <qspinbox.h>
#include <qmessagebox.h>

KMPlayerPreferences::KMPlayerPreferences(QWidget *parent)
: KDialogBase(TreeList, i18n("KMPlayer Preferences"),
		Help|Default|Ok|Apply|Cancel, Ok, parent, 0, true)
{
	QFrame *frame;
	QStringList hierarcy; // typo? :)
	QVBoxLayout *vlay;
	

	hierarcy << i18n("General") << i18n("General");
	frame = addPage(hierarcy, i18n("General Options"));
	vlay = new QVBoxLayout(frame, marginHint(), spacingHint());
	
	m_GeneralPageGeneral = new KMPlayerPrefGeneralPageGeneral(frame);
	vlay->addWidget(m_GeneralPageGeneral);
	

	hierarcy.clear();
	hierarcy << i18n("General") << i18n("DVD");
	frame = addPage(hierarcy, i18n("DVD playing options"));
	vlay = new QVBoxLayout(frame, marginHint(), spacingHint());
	m_GeneralPageDVD = new KMPlayerPrefGeneralPageDVD(frame);
	vlay->addWidget(m_GeneralPageDVD);
	

	hierarcy.clear();
	hierarcy << i18n("General") << i18n("VCD");
	frame = addPage(hierarcy, i18n("VCD playing options"));
	vlay = new QVBoxLayout(frame, marginHint(), spacingHint());
	m_GeneralPageVCD = new KMPlayerPrefGeneralPageVCD(frame);
	vlay->addWidget(m_GeneralPageVCD);


	hierarcy.clear();
	hierarcy << i18n("General") << i18n("Output");
	frame = addPage(hierarcy, i18n("Video and audio output options || NOT YET USED == FIXME!"));
	vlay = new QVBoxLayout(frame, marginHint(), spacingHint());
	m_GeneralPageOutput = new KMPlayerPrefGeneralPageOutput(frame);
	vlay->addWidget(m_GeneralPageOutput);
	
	hierarcy.clear();
	hierarcy << i18n("General") << i18n("Advanced");
	frame = addPage(hierarcy, i18n("Advanced options"));
	vlay = new QVBoxLayout(frame, marginHint(), spacingHint());
	m_GeneralPageAdvanced = new KMPlayerPrefGeneralPageAdvanced(frame);
	vlay->addWidget(m_GeneralPageAdvanced);

/*
* not yet needed...
 */
 
	/*hierarcy.clear();
	hierarcy << i18n("Output plugins") << i18n("General");
	frame = addPage(hierarcy, i18n("Output plugin Options || NOT YET USED == FIXME!"));
	vlay = new QVBoxLayout(frame, marginHint(), spacingHint());
	m_OPPageGeneral = new KMPlayerPrefOPPageGeneral(frame);
	vlay->addWidget(m_OPPageGeneral);*/
	
	
	hierarcy.clear();
	hierarcy << i18n("Output plugins") << i18n("Postprocessing");
	frame = addPage(hierarcy, i18n("Postprocessing options"));
	vlay = new QVBoxLayout(frame, marginHint(), spacingHint());
	m_OPPagePostproc = new KMPlayerPrefOPPagePostProc(frame);
	vlay->addWidget(m_OPPagePostproc);
	
	connect(this, SIGNAL( defaultClicked() ), SLOT( confirmDefaults() ));
	this->setTreeListAutoResize(true);
}

KMPlayerPreferences::~KMPlayerPreferences() {}

KMPlayerPrefGeneralPageGeneral::KMPlayerPrefGeneralPageGeneral(QWidget *parent)
: QFrame(parent)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	keepSizeRatio = new QCheckBox (i18n("Keep size ratio"), this, 0);
	QToolTip::add(keepSizeRatio, i18n("When checked, movie will keep it's aspect ratio\nwhen window is resized"));
	showConsoleOutput = new QCheckBox (i18n("Show console output"), this, 0);
	QToolTip::add(showConsoleOutput, i18n("Shows output from mplayer before and after playing the movie"));
	loop = new QCheckBox (i18n("Loop"), this, 0);
	QToolTip::add(loop, i18n("Makes current movie loop"));
	showControlButtons = new QCheckBox (i18n("Show control buttons"), this, 0);
	QToolTip::add(showControlButtons, i18n("Small buttons will be shown above statusbar to control movie"));
	autoHideControlButtons = new QCheckBox (i18n("Auto hide control buttons"), this, 0);
	QToolTip::add(autoHideControlButtons, i18n("When checked, control buttons will get hidden automatically"));
	showPositionSlider	= new QCheckBox (i18n("Show position slider"), this, 0);
	QToolTip::add(showPositionSlider, i18n("When enabled, will show seeking slider above control buttons"));
	//autoHideSlider = new QCheckBox (i18n("Auto hide position slider"), this, 0);
	alwaysBuildIndex = new QCheckBox ( i18n("Build new index when possible"), this);
	QToolTip::add(alwaysBuildIndex, i18n("Allows seeking in indexed files (AVIs)"));

	QWidget *seekingWidget = new QWidget(this);
	QHBoxLayout *seekingWidgetLayout = new QHBoxLayout(seekingWidget);
	seekingWidgetLayout->addWidget(new QLabel(i18n("Forward/backward seek time"),seekingWidget));
	seekingWidgetLayout->addItem(new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Minimum));
	seekTime = new QSpinBox(1, 600, 1, seekingWidget);
	seekingWidgetLayout->addWidget(seekTime);
	seekingWidgetLayout->addItem(new QSpacerItem(0,0,QSizePolicy::Minimum, QSizePolicy::Minimum));
	layout->addWidget(keepSizeRatio);
	layout->addWidget(showConsoleOutput);
	layout->addWidget(loop);
	layout->addWidget(showControlButtons);
	layout->addWidget(autoHideControlButtons);
	layout->addWidget(showPositionSlider);
	//layout->addWidget(autoHideSlider);
	layout->addWidget(alwaysBuildIndex);
	layout->addWidget(seekingWidget);
}


KMPlayerPrefGeneralPageDVD::KMPlayerPrefGeneralPageDVD(QWidget *parent) : QFrame(parent)
{
	QVBoxLayout *layout 	= new QVBoxLayout (this);


	autoPlayDVD 		= new QCheckBox (i18n("Auto play after opening DVD"), this, 0);
	QToolTip::add(autoPlayDVD, i18n("FIXME! Start playing DVD right after opening DVD")); // i don't know about this

	QLabel *dvdDevicePathLabel = new QLabel (i18n("DVD device:"), this, 0);
	dvdDevicePath 		= new QLineEdit ("/dev/dvd", this, 0);
	QToolTip::add(dvdDevicePath, i18n("Path to your DVD device, you must have read rights to this device")); 
	layout->addWidget(autoPlayDVD);
	layout->addWidget(dvdDevicePathLabel);
	layout->addWidget(dvdDevicePath);
	layout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum ) );

}

KMPlayerPrefGeneralPageVCD::KMPlayerPrefGeneralPageVCD(QWidget *parent) : QFrame(parent)
{
	QVBoxLayout *layout = new QVBoxLayout (this);


	autoPlayVCD = new QCheckBox (i18n("Auto play after opening a VCD"), this, 0);
	QToolTip::add(autoPlayVCD, i18n("FIXME! Start playing VCD right after opening VCD")); // i don't know about this

	QLabel *vcdDevicePathLabel = new QLabel (i18n("VCD (CDROM) device:"), this, 0);
	vcdDevicePath = new QLineEdit ("/dev/cdrom", this, 0);
	QToolTip::add(vcdDevicePath, i18n("Path to your CDROM/DVD device, you must have read rights to this device"));

	QSpacerItem* spacer = new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum );
	layout->addWidget(autoPlayVCD);
	layout->addWidget(vcdDevicePathLabel);
	layout->addWidget(vcdDevicePath);
	layout->addItem( spacer );
}

KMPlayerPrefGeneralPageOutput::KMPlayerPrefGeneralPageOutput(QWidget *parent) : QFrame(parent)
{
	QVBoxLayout *layout = new QVBoxLayout (this);
	QHBoxLayout *childLayout1 = new QHBoxLayout (layout);
	
	videoDriver = new QComboBox(this);
	videoDriver->insertItem(VDRIVER_XV, VDRIVER_XV_INDEX);
	videoDriver->insertItem(VDRIVER_X11, VDRIVER_X11_INDEX);
	videoDriver->insertItem(VDRIVER_XVIDIX, VDRIVER_XVIDIX_INDEX);
	QToolTip::add(videoDriver, i18n("Sets video driver, currently only XVideo and X11 work. Unless\nyou haven't got XVideo compatible drivers you should X11, which is much slower."));
	childLayout1->addWidget(new QLabel(i18n("Video driver:"),this));
	childLayout1->addWidget(videoDriver);
	
	QHBoxLayout *childLayout2 = new QHBoxLayout (layout);
	audioDriver = new QComboBox(this);
	audioDriver->insertItem(ADRIVER_DEFAULT, ADRIVER_DEFAULT_INDEX);
	audioDriver->insertItem(ADRIVER_OSS, ADRIVER_OSS_INDEX);
	audioDriver->insertItem(ADRIVER_SDL, ADRIVER_SDL_INDEX);
	audioDriver->insertItem(ADRIVER_ALSA, ADRIVER_ALSA_INDEX);
	audioDriver->insertItem(ADRIVER_ARTS, ADRIVER_ARTS_INDEX);
	childLayout2->addWidget(new QLabel(i18n("Audio driver:"),this));
	childLayout2->addWidget(audioDriver);
	
}

KMPlayerPrefOPPageGeneral::KMPlayerPrefOPPageGeneral(QWidget *parent)
: QFrame(parent)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setAutoAdd(true);
}

KMPlayerPrefOPPagePostProc::KMPlayerPrefOPPagePostProc(QWidget *parent) : QFrame(parent)
{
	
	QVBoxLayout *tabLayout = new QVBoxLayout (this);
	postProcessing = new QCheckBox( i18n("postProcessing"), this, 0 );
	postProcessing->setEnabled( TRUE );
	disablePPauto = new QCheckBox (i18n("disablePostProcessingAutomatically"), this, 0);
	
	tabLayout->addWidget( postProcessing );
	tabLayout->addWidget( disablePPauto );
	tabLayout->addItem ( new QSpacerItem( 5, 5, QSizePolicy::Minimum, QSizePolicy::Minimum ) );
	
	PostprocessingOptions = new QTabWidget( this, "PostprocessingOptions" );
	PostprocessingOptions->setEnabled( TRUE );
	PostprocessingOptions->setAutoMask( FALSE );
	PostprocessingOptions->setTabPosition( QTabWidget::Top );
	PostprocessingOptions->setTabShape( QTabWidget::Rounded );
	PostprocessingOptions->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, PostprocessingOptions->sizePolicy().hasHeightForWidth() ) );

	QWidget *presetSelectionWidget = new QWidget( PostprocessingOptions, "presetSelectionWidget" );
	QGridLayout *presetSelectionWidgetLayout = new QGridLayout( presetSelectionWidget, 1, 1, 1);

	QButtonGroup *presetSelection = new QButtonGroup(3, Qt::Vertical, presetSelectionWidget);
	presetSelection->setInsideSpacing(KDialog::spacingHint());

	defaultPreset = new QRadioButton( presetSelection, "defaultPreset" );
	defaultPreset->setChecked( TRUE );
	int defaultPresetID = presetSelection->insert(defaultPreset);

	customPreset = new QRadioButton( presetSelection, "customPreset" );
	int customPresetID = presetSelection->insert(customPreset);

	fastPreset = new QRadioButton( presetSelection, "fastPreset" );
	int fastPresetID = presetSelection->insert(fastPreset);
	presetSelection->setRadioButtonExclusive ( true);
	presetSelectionWidgetLayout->addWidget( presetSelection, 0, 0 );
	PostprocessingOptions->insertTab( presetSelectionWidget, "" );

	//
	// SECOND!!!
	//
	/* I JUST WASN'T ABLE TO GET THIS WORKING WITH QGridLayouts */

	QWidget *customFiltersWidget = new QWidget( PostprocessingOptions, "customFiltersWidget" ); 
	QVBoxLayout *customFiltersWidgetLayout = new QVBoxLayout( customFiltersWidget );

	QGroupBox *customFilters = new QGroupBox(0, Qt::Vertical, customFiltersWidget, "customFilters" );
	customFilters->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)1, (QSizePolicy::SizeType)2));
	customFilters->setFlat(false);
	customFilters->setEnabled( FALSE );
	customFilters->setInsideSpacing(7);
	
	QLayout *customFiltersLayout = customFilters->layout();
	QHBoxLayout *customFiltersLayout1 = new QHBoxLayout ( customFilters->layout() );

	HzDeblockFilter = new QCheckBox( customFilters, "HzDeblockFilter" );
	HzDeblockAQuality = new QCheckBox( customFilters, "HzDeblockAQuality" );
	HzDeblockAQuality->setEnabled( FALSE );
	HzDeblockCFiltering = new QCheckBox( customFilters, "HzDeblockCFiltering" );
	HzDeblockCFiltering->setEnabled( FALSE );

	customFiltersLayout1->addWidget( HzDeblockFilter );
	customFiltersLayout1->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum ) );
	customFiltersLayout1->addWidget( HzDeblockAQuality );
	customFiltersLayout1->addWidget( HzDeblockCFiltering );

	QFrame *line1 = new QFrame( customFilters, "line1" );
	line1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)2 ) );
	line1->setFrameShape( QFrame::HLine );
	line1->setFrameShadow( QFrame::Sunken );
	customFiltersLayout->add(line1);

	QHBoxLayout *customFiltersLayout2 = new QHBoxLayout ( customFilters->layout() );

	VtDeblockFilter = new QCheckBox( customFilters, "VtDeblockFilter" );
	VtDeblockAQuality = new QCheckBox( customFilters, "VtDeblockAQuality" );
	VtDeblockAQuality->setEnabled( FALSE );
	VtDeblockCFiltering = new QCheckBox( customFilters, "VtDeblockCFiltering" );
	VtDeblockCFiltering->setEnabled( FALSE );

	customFiltersLayout2->addWidget( VtDeblockFilter );
	customFiltersLayout2->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum ) );
	customFiltersLayout2->addWidget( VtDeblockAQuality );
	customFiltersLayout2->addWidget( VtDeblockCFiltering );

	QFrame *line2 = new QFrame( customFilters, "line2" );

	line2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)2 ) );
	line2->setFrameShape( QFrame::HLine );
	line2->setFrameShadow( QFrame::Sunken );
	customFiltersLayout->add(line2);

	QHBoxLayout *customFiltersLayout3  = new QHBoxLayout ( customFilters->layout() );

	DeringFilter = new QCheckBox( customFilters, "DeringFilter" );
	DeringAQuality = new QCheckBox( customFilters, "DeringAQuality" );
	DeringAQuality->setEnabled( FALSE );
	DeringCFiltering = new QCheckBox( customFilters, "DeringCFiltering" );
	DeringCFiltering->setEnabled( FALSE );

	customFiltersLayout3->addWidget( DeringFilter );
	customFiltersLayout3->addItem( new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum ) );
	customFiltersLayout3->addWidget( DeringAQuality );
	customFiltersLayout3->addWidget( DeringCFiltering );


	QFrame *line3 = new QFrame( customFilters, "line3" );

	line3->setFrameShape( QFrame::HLine );
	line3->setFrameShadow( QFrame::Sunken );
	line3->setFrameShape( QFrame::HLine );

	customFiltersLayout->add(line3);

	QHBoxLayout *customFiltersLayout4 = new QHBoxLayout (customFilters->layout());

	AutolevelsFilter = new QCheckBox( customFilters, "AutolevelsFilter" );
	AutolevelsFullrange = new QCheckBox( customFilters, "AutolevelsFullrange" );
	AutolevelsFullrange->setEnabled( FALSE );

	customFiltersLayout4->addWidget(AutolevelsFilter);
	customFiltersLayout4->addItem(new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum ));
	customFiltersLayout4->addWidget(AutolevelsFullrange);

	QHBoxLayout *customFiltersLayout5 = new QHBoxLayout (customFilters->layout());

	TmpNoiseFilter = new QCheckBox( customFilters, "TmpNoiseFilter" );
/*	TmpNoiseSlider = new QSlider( customFilters, "TmpNoiseSlider" );
	TmpNoiseSlider->setEnabled( FALSE );
	TmpNoiseSlider->setMinValue( 1 );
	TmpNoiseSlider->setMaxValue( 3 );
	TmpNoiseSlider->setValue( 1 );
	TmpNoiseSlider->setOrientation( QSlider::Horizontal );
	TmpNoiseSlider->setTickmarks( QSlider::Left );
	TmpNoiseSlider->setTickInterval( 1 );
	TmpNoiseSlider->setSizePolicy(QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1));*/

	/*customFiltersLayout->addWidget(TmpNoiseFilter,7,0);
	customFiltersLayout->addWidget(TmpNoiseSlider,7,2);*/
	customFiltersLayout5->addWidget(TmpNoiseFilter);
	customFiltersLayout5->addItem(new QSpacerItem( 0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum ));
	//customFiltersLayout5->addWidget(TmpNoiseSlider);
	customFiltersWidgetLayout->addWidget( customFilters );
	PostprocessingOptions->insertTab( customFiltersWidget, "" );
	//
	//THIRD!!!
	//
	QWidget *deintSelectionWidget = new QWidget( PostprocessingOptions, "deintSelectionWidget" );
	QVBoxLayout *deintSelectionWidgetLayout = new QVBoxLayout( deintSelectionWidget); 
	QButtonGroup *deinterlacingGroup = new QButtonGroup(5, Qt::Vertical, deintSelectionWidget, "deinterlacingGroup" );

	LinBlendDeinterlacer = new QCheckBox( deinterlacingGroup, "LinBlendDeinterlacer" );
	LinIntDeinterlacer = new QCheckBox( deinterlacingGroup, "LinIntDeinterlacer" );
	CubicIntDeinterlacer = new QCheckBox( deinterlacingGroup, "CubicIntDeinterlacer" );
	MedianDeinterlacer = new QCheckBox( deinterlacingGroup, "MedianDeinterlacer" );
	FfmpegDeinterlacer = new QCheckBox( deinterlacingGroup, "FfmpegDeinterlacer" );

	int LinBlendDeinterlacerID = deinterlacingGroup->insert( LinBlendDeinterlacer );

	int LinIntDeinterlacerID = deinterlacingGroup->insert( LinIntDeinterlacer );

	int CubicIntDeinterlacerID = deinterlacingGroup->insert( CubicIntDeinterlacer );

	int MedianDeinterlacerID = deinterlacingGroup->insert( MedianDeinterlacer );

	int FfmpegDeinterlacerID = deinterlacingGroup->insert( FfmpegDeinterlacer );
	
	
	deintSelectionWidgetLayout->addWidget( deinterlacingGroup, 0, 0 );

	PostprocessingOptions->insertTab( deintSelectionWidget, "" );

	tabLayout->addWidget( PostprocessingOptions/*, 1, 0*/ );

	PostprocessingOptions->setEnabled(false);
	connect( customPreset, SIGNAL (toggled(bool) ), customFilters, SLOT(setEnabled(bool)));
	connect( postProcessing, SIGNAL( toggled(bool) ), PostprocessingOptions, SLOT( setEnabled(bool) ) );
	connect( HzDeblockFilter, SIGNAL( toggled(bool) ), HzDeblockAQuality, SLOT( setEnabled(bool) ) );
	connect( HzDeblockFilter, SIGNAL( toggled(bool) ), HzDeblockCFiltering, SLOT( setEnabled(bool) ) );
	connect( VtDeblockFilter, SIGNAL( toggled(bool) ), VtDeblockCFiltering, SLOT( setEnabled(bool) ) );
	connect( VtDeblockFilter, SIGNAL( toggled(bool) ), VtDeblockAQuality, SLOT( setEnabled(bool) ) );
	connect( DeringFilter, SIGNAL( toggled(bool) ), DeringAQuality, SLOT( setEnabled(bool) ) );
	connect( DeringFilter, SIGNAL( toggled(bool) ), DeringCFiltering, SLOT( setEnabled(bool) ) );
	//connect( TmpNoiseFilter, SIGNAL( toggled(bool) ), TmpNoiseSlider, SLOT( setEnabled(bool) ) );
	
	connect( AutolevelsFilter, SIGNAL( toggled(bool) ), AutolevelsFullrange, SLOT( setEnabled(bool) ) );

	postProcessing->setText( i18n( "Enable use of postprocessing filters" ) );
	disablePPauto->setText( i18n( "Disable use of postprocessing when watching TV/DVD" ) );
	defaultPreset->setText( i18n( "Default" ) );
	QToolTip::add( defaultPreset, i18n( "Enable mplayer's default postprocessing filters" ) );
	customPreset->setText( i18n( "Custom" ) );
	QToolTip::add( customPreset, i18n( "Enable custom postprocessing filters (See: Custom preset -tab)" ) );
	fastPreset->setText( i18n( "Fast" ) );
	QToolTip::add( fastPreset, i18n( "Enable mplayer's fast postprocessing filters" ) );
	PostprocessingOptions->changeTab( presetSelectionWidget, i18n( "General" ) );
	customFilters->setTitle( QString::null );
	HzDeblockFilter->setText( i18n( "Horizontal deblocking" ) );
	VtDeblockFilter->setText( i18n( "Vertical deblocking" ) );
	DeringFilter->setText( i18n( "Dering filter" ) );
	HzDeblockAQuality->setText( i18n( "Auto quality" ) );
	QToolTip::add( HzDeblockAQuality, i18n( "Filter is used if there's enough CPU" ) );
	VtDeblockAQuality->setText( i18n( "Auto quality" ) );
	QToolTip::add( VtDeblockAQuality, i18n( "Filter is used if there's enough CPU" ) );
	DeringAQuality->setText( i18n( "Auto quality" ) );
	QToolTip::add( DeringAQuality, i18n( "Filter is used if there's enough CPU" ) );
	//QToolTip::add( TmpNoiseSlider, i18n( "Strength of the noise reducer" ) );
	AutolevelsFilter->setText( i18n( "Auto brightness/contrast" ) );
	AutolevelsFullrange->setText( i18n( "Strech luminance to full range" ) );
	QToolTip::add( AutolevelsFullrange, i18n( "Streches luminance to full range (0..255)" ) );
	HzDeblockCFiltering->setText( i18n( "Chrominance filtering" ) );
	VtDeblockCFiltering->setText( i18n( "Chrominance filtering" ) );
	DeringCFiltering->setText( i18n( "Chrominance filtering" ) );
	TmpNoiseFilter->setText( i18n( "Temporal noise reducer:" ) );
	PostprocessingOptions->changeTab( customFiltersWidget, i18n( "Custom Preset" ) );
	deinterlacingGroup->setTitle( QString::null );
	LinBlendDeinterlacer->setText( i18n( "Linear blend deinterlacer" ) );
	CubicIntDeinterlacer->setText( i18n( "Cubic interpolating deinterlacer" ) );
	LinIntDeinterlacer->setText( i18n( "Linear interpolating deinterlacer" ) );
	MedianDeinterlacer->setText( i18n( "Median deinterlacer" ) );
	FfmpegDeinterlacer->setText( i18n( "FFmpeg deinterlacer" ) );
	PostprocessingOptions->changeTab( deintSelectionWidget, i18n( "Deinterlacing" ) );
	PostprocessingOptions->adjustSize();
}
KMPlayerPrefGeneralPageAdvanced::KMPlayerPrefGeneralPageAdvanced(QWidget *parent) : QFrame(parent)
{
	QVBoxLayout *layout = new QVBoxLayout (this);
	layout->setSpacing(2);
	QGroupBox *realGroupBox = new QGroupBox ( i18n("Pattern matching"), this, "realGroupBox");

	realGroupBox->setFlat( false );
	realGroupBox->setInsideMargin( 7 );
	QVBoxLayout *realGroupBoxLayout = new QVBoxLayout (realGroupBox->layout());

	QGridLayout *groupBoxLayout = new QGridLayout (realGroupBoxLayout,1,1);
	
	QLabel *langPattLabel = new QLabel (i18n("DVD Language pattern:"), realGroupBox, 0);
	dvdLangPattern = new QLineEdit (realGroupBox);
	groupBoxLayout->addWidget(langPattLabel,0,0);
	groupBoxLayout->addWidget(dvdLangPattern,0,2);

	QLabel *titlePattLabel = new QLabel (i18n("DVD Titles pattern:"), realGroupBox, 0);
	dvdTitlePattern = new QLineEdit (realGroupBox);
	groupBoxLayout->addWidget(titlePattLabel,1,0);
	groupBoxLayout->addWidget(dvdTitlePattern,1,2);

	QLabel *subPattLabel = new QLabel (i18n("DVD Subtitle pattern:"), realGroupBox, 0);
	dvdSubPattern = new QLineEdit (realGroupBox);
	groupBoxLayout->addWidget(subPattLabel,2,0);
	groupBoxLayout->addWidget(dvdSubPattern,2,2);

	QLabel *chapPattLabel = new QLabel (i18n("DVD Chapters pattern:"), realGroupBox, 0);
	dvdChapPattern = new QLineEdit (realGroupBox);
	groupBoxLayout->addWidget(chapPattLabel,3,0);
	groupBoxLayout->addWidget(dvdChapPattern,3,2);
	
	QLabel *trackPattLabel = new QLabel (i18n("VCD Track pattern:"), realGroupBox, 0);
	vcdTrackPattern = new QLineEdit (realGroupBox);
	groupBoxLayout->addWidget(trackPattLabel,4,0);
	groupBoxLayout->addWidget(vcdTrackPattern,4,2);
	
	QLabel *sizePattLabel = new QLabel (i18n("Size pattern:"), realGroupBox, 0);
	sizePattern = new QLineEdit (realGroupBox);
	groupBoxLayout->addWidget(sizePattLabel,5,0);
	groupBoxLayout->addWidget(sizePattern,5,2);
	
	QLabel *cachePattLabel = new QLabel (i18n("Cache pattern:"), realGroupBox, 0);
	cachePattern = new QLineEdit (realGroupBox);
	groupBoxLayout->addWidget(cachePattLabel,6,0);
	groupBoxLayout->addWidget(cachePattern,6,2);

	QLabel *indexPattLabel = new QLabel (i18n("Index pattern:"), realGroupBox, 0);
	indexPattern = new QLineEdit (realGroupBox);
	groupBoxLayout->addWidget(indexPattLabel,7,0);
	groupBoxLayout->addWidget(indexPattern,7,2);
	
	QLabel *startPattLabel = new QLabel (i18n("Start pattern:"), realGroupBox, 0);
	startPattern = new QLineEdit (realGroupBox);
	groupBoxLayout->addWidget(startPattLabel,8,0);
	groupBoxLayout->addWidget(startPattern,8,2);

	groupBoxLayout->addColSpacing(1, 10);
	layout->addWidget(realGroupBox);
	

	layout->addWidget(new QLabel (i18n("Additional command line arguments:"),this));
	additionalArguments = new QLineEdit(this);
	layout->addWidget(additionalArguments);
	
	
	QHBoxLayout *addLayout2 = new QHBoxLayout (layout);
	addLayout2->addWidget(new QLabel (i18n("Cache size:"),this));
	cacheSize = new QSpinBox (0, 32767, 32, this);
	addLayout2->addWidget(cacheSize);
	addLayout2->addWidget(new QLabel (i18n("kB"),this));
	
	layout->addItem(new QSpacerItem(1,1, QSizePolicy::Minimum, QSizePolicy::Minimum));
}

void KMPlayerPreferences::confirmDefaults() {
	switch( QMessageBox::warning( this, "KMPlayer",
        i18n("You are about to have all your settings overwritten with defaults.\nPlease confirm.\n"),
        i18n("Ok"), i18n("Cancel"), QString::null, 0, 1 ) ){
    		case 0:	KMPlayerPreferences::setDefaults();
        		break;
    		case 1:	break;
	}	
	
}

void KMPlayerPreferences::setDefaults() {
	m_GeneralPageGeneral->keepSizeRatio->setChecked(true);
	m_GeneralPageGeneral->showConsoleOutput->setChecked(false);
	m_GeneralPageGeneral->loop->setChecked(false);
	m_GeneralPageGeneral->showControlButtons->setChecked(true);
	m_GeneralPageGeneral->autoHideControlButtons->setChecked(false);
	m_GeneralPageGeneral->showPositionSlider->setChecked(true);
	m_GeneralPageGeneral->seekTime->setValue(10);
	
	m_GeneralPageDVD->autoPlayDVD->setChecked(true);
	m_GeneralPageDVD->dvdDevicePath->setText("/dev/dvd");
	
	m_GeneralPageVCD->autoPlayVCD->setChecked(true);
	m_GeneralPageVCD->vcdDevicePath->setText("/dev/cdrom");
	
	m_GeneralPageOutput->videoDriver->setCurrentItem(VDRIVER_XV_INDEX);
	m_GeneralPageOutput->audioDriver->setCurrentItem(ADRIVER_DEFAULT_INDEX);
	
	m_GeneralPageAdvanced->dvdLangPattern->setText("\\[open].*audio.*language: ([A-Za-z]+).*aid.*[^0-9]([0-9]+)");
	m_GeneralPageAdvanced->dvdTitlePattern->setText("There are ([0-9]+) titles");
	m_GeneralPageAdvanced->dvdSubPattern->setText("\\[open].*subtitle.*[^0-9]([0-9]+).*language: ([A-Za-z]+)");
	m_GeneralPageAdvanced->dvdChapPattern->setText("There are ([0-9]+) chapters");
	m_GeneralPageAdvanced->vcdTrackPattern->setText("track ([0-9]+):");
	m_GeneralPageAdvanced->sizePattern->setText("VO:.*[^0-9]([0-9]+)x([0-9]+)");
	m_GeneralPageAdvanced->cachePattern->setText("Cache fill:[^0-9]*([0-9\\.]+)%");
	m_GeneralPageAdvanced->startPattern->setText("Start[^ ]* play");
	m_GeneralPageAdvanced->additionalArguments->setText("");
	m_GeneralPageAdvanced->cacheSize->setValue(256);
	
	m_OPPagePostproc->postProcessing->setChecked(false);
	m_OPPagePostproc->disablePPauto->setChecked(true);
	
	m_OPPagePostproc->defaultPreset->setChecked(true);
	
	m_OPPagePostproc->LinBlendDeinterlacer->setChecked(false);
	m_OPPagePostproc->LinIntDeinterlacer->setChecked(false);
	m_OPPagePostproc->CubicIntDeinterlacer->setChecked(false);
	m_OPPagePostproc->MedianDeinterlacer->setChecked(false);
	m_OPPagePostproc->FfmpegDeinterlacer->setChecked(false);
	
}
#include "pref.moc"
