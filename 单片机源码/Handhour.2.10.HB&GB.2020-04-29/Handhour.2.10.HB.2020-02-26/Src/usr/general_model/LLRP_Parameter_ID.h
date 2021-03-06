#ifndef __LLRP_PARAMETER_ID_H__
#define __LLRP_PARAMETER_ID_H__

#define PARAM_BASE 10000

#define Status PARAM_BASE+300
#define UTCTimestamp PARAM_BASE+301
#define GPIEvent PARAM_BASE+302
#define SelectSpecEvent PARAM_BASE+303
#define AntennaSpecEvent PARAM_BASE+304
#define AntennaStatusEvent PARAM_BASE+305
#define ConnectionAttemptEvent PARAM_BASE+306
#define GenaralCapabilities PARAM_BASE+350
#define GPIOCapabilities PARAM_BASE+351
#define CommunicationCapabilities PARAM_BASE+352
#define SupportRS232 PARAM_BASE+353
#define SerialAttribute PARAM_BASE+354
#define SupportRS485 PARAM_BASE+355
#define SpecCapabilities PARAM_BASE+356
#define RfCapabilities PARAM_BASE+357
#define TransmitPowerLevelTable PARAM_BASE+358
#define FrequencyInformation PARAM_BASE+359
#define FrequencyTable PARAM_BASE+360
#define ForDataRateTable PARAM_BASE+361
#define RevDataRateTable PARAM_BASE+362
#define ForModulationTable PARAM_BASE+363
#define RevDataEncodingTable PARAM_BASE+364
#define ModuleDepthTable PARAM_BASE+365
#define AirProtocolCapabilities PARAM_BASE+366
#define PerAntennaAirProtocol PARAM_BASE+367
#define HbProtocolCapabilities PARAM_BASE+368
//#define SecurityModuleCapabilities PARAM_BASE+369
#define SelectSpec PARAM_BASE+400
#define SelectSpecStartTrigger PARAM_BASE+401
#define PeriodicTrigger PARAM_BASE+402
#define GPITrigger PARAM_BASE+403
#define SelectSpecStopTrigger PARAM_BASE+404
#define AntennaSpec PARAM_BASE+405
#define AntennaSpecStopTrigger PARAM_BASE+406
#define TagObservationTrigger PARAM_BASE+407
#define RfSpec PARAM_BASE+408
#define MemoryBank PARAM_BASE+409
#define SelectReportSpec PARAM_BASE+410
#define ReportDestination PARAM_BASE+411
#define AccessSpec PARAM_BASE+450
#define AccessSpecStopTrigger PARAM_BASE+451
#define AccessCommand PARAM_BASE+452
#define HbMatchSpec PARAM_BASE+453
#define HbTargetTag PARAM_BASE+454
#define HbReadSpec PARAM_BASE+455
#define HbWriteSpec PARAM_BASE+458
#define HbPrivateWriteSpec PARAM_BASE+459
#define ClientRequestSpec PARAM_BASE+456
#define AccessReportSpec PARAM_BASE+457
#define SecurityModuleSpec PARAM_BASE+460
#define TagReportData PARAM_BASE+500
#define SelectSpecID PARAM_BASE+501
#define SpecIndex PARAM_BASE+502
#define RfSpecID PARAM_BASE+503
#define AntennaID PARAM_BASE+504
#define PeakRSSI PARAM_BASE+505
#define FirstSeenTimestampUTC PARAM_BASE+506
#define LastSeenTimestampUTC PARAM_BASE+507
#define TagSeenCount PARAM_BASE+508
#define AccessSpecID PARAM_BASE+509
#define GenaralSelectSpecResult PARAM_BASE+510
#define HbReadSpecResult PARAM_BASE+511
#define HbWriteSpecResult PARAM_BASE+513
#define HbPrivateWriteSpecResult PARAM_BASE+514
#define ClientRequestSpecResult PARAM_BASE+512
#define HbCustomizedReadSpecResult PARAM_BASE+515
#define ReadDataInfo PARAM_BASE+516
#define CustomizedSelectSpecResult PARAM_BASE+517
#define EnableCachedEventsAndReport PARAM_BASE+580
#define CachedSelectAccessReport PARAM_BASE+581
#define CachedSelectAccessReportAck PARAM_BASE+582
#define CID PARAM_BASE+1001
#define FPDH PARAM_BASE+1002
#define SYXZ PARAM_BASE+1003
#define CCRQ PARAM_BASE+1004
#define CLLX PARAM_BASE+1005
#define GL PARAM_BASE+1006
#define PL PARAM_BASE+1007
#define HPZL PARAM_BASE+1008
#define HPHMXH PARAM_BASE+1009
#define JYYXQ PARAM_BASE+1010
#define QZBFQ PARAM_BASE+1011
#define ZKZL PARAM_BASE+1012
#define CSYS PARAM_BASE+1013
#define ClientRequestAck PARAM_BASE+590
#define TagLog PARAM_BASE+620
#define StartLogTimestamp PARAM_BASE+621
#define EndLogTimestamp PARAM_BASE+622
#define DeviceLog PARAM_BASE+640
#define DeviceEventNotificationSpec PARAM_BASE+660
#define EventNotificationState PARAM_BASE+661
#define AntennaProperties PARAM_BASE+662
#define AntennaConfiguration PARAM_BASE+663
#define ModuleDepth PARAM_BASE+664
#define Identification PARAM_BASE+665
#define AlarmConfiguration PARAM_BASE+666
#define AlarmThreshod PARAM_BASE+667
#define CommunicationConfiguration PARAM_BASE+668
#define CommLinkConfiguration PARAM_BASE+669
#define KeepaliveSpec PARAM_BASE+670
#define TcpLinkConfiguration PARAM_BASE+671
#define ClientModeConfiguration PARAM_BASE+672
#define IPAddress PARAM_BASE+673
#define ServerModeConfiguration PARAM_BASE+674
#define SerialLinkConfiguration PARAM_BASE+675
#define HttpLinkConfiguration PARAM_BASE+676
#define EthernetIpv4Configuration PARAM_BASE+677
#define EthernetIpv6Configuration PARAM_BASE+678
#define NTPConfiguration PARAM_BASE+679
#define SerialPortConfiguration PARAM_BASE+680
#define SecurityModuleConfiguration PARAM_BASE+684
#define GenaralConfigData PARAM_BASE+685
#define RTCTime PARAM_BASE+686
#define SecurityModuleSN PARAM_BASE+687
#define ReadMode PARAM_BASE+688
//#define PasswordVersion PARAM_BASE+689
#define VersionInfo PARAM_BASE+700
#define VersionDownload PARAM_BASE+701
#define AlarmReportInfo PARAM_BASE+720
#define AlarmRestoreInfo PARAM_BASE+721
#define AlarmSyncInfo PARAM_BASE+722
#define DiagnosticTestItem PARAM_BASE+740
#define DiagnosticTestResultItem PARAM_BASE+741
#define DiagnosticTestResultAntennaConnected PARAM_BASE+742
#define DiagnosticTestResultAntennaVSWR PARAM_BASE+743

#define LocationConfiguration PARAM_BASE+681
#define GpsLocation PARAM_BASE+682
#define BdsLocation PARAM_BASE+683


#endif
