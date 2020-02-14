typedef const char * const NP2_KEYWORDS[4];

// JavaDoc
static NP2_KEYWORDS kwJavaDoc = {
"author code docRoot deprecated exception inheritDoc link linkplain literal param "
"return see serial serialData serialField since throws value version "
"hide "
, NULL, NULL, NULL
};

// PHPDoc
static NP2_KEYWORDS kwPHPDoc = {
"abstract access api author category copyright deprecated example filesource final "
"global id ignore inheritdoc internal license link method name package param "
"property property-read property-write return see since source static staticvar "
"subpackage toc todo throws tutorial type uses var version  "
, // pesudo type
"resource mixed number callback void scalar "
,
"readonly", NULL
};

// C# Doc
static NP2_KEYWORDS kwNETDoc = {
"code example exception include inheritdoc list para param paramref permission remarks returns "
"see seealso summary typeparam typeparamref value listheader item term description ",
"cref file path type name "
, // attribute
"bullet number table"
, NULL
};

// Doxygen
static NP2_KEYWORDS kwDoxyDoc = {
"a addindex addtogroup anchor arg attention author authors b brief bug c callgraph callergraph category cite class code cond copybrief copydetails copydoc copyright date def defgroup deprecated details dir docbookonly dontinclude dot dotfile e else elseif em endcode endcond enddocbookonly enddot endhtmlonly endif endinternal endlatexonly endlink endmanonly endmsc endrtfonly endsecreflist endverbatim endxmlonly enum example exception extends file fn headerfile hideinitializer htmlinclude htmlonly idlexcept if ifnot image implements include includelineno ingroup internal invariant interface latexonly li line link mainpage manonly memberof msc mscfile n name namespace nosubgrouping note overload p package page par paragraph param post pre private privatesection property protected protectedsection protocol public publicsection pure ref refitem related relates relatedalso relatesalso remark remarks result return returns retval rtfonly sa secreflist section see short showinitializer since skip skipline snippet struct subpage subsection subsubsection tableofcontents test throw throws todo tparam typedef union until var verbatim verbinclude version vhdlflow warning weakgroup xmlonly xrefitem "
, // C#
"inheritdoc list description item term listheader para paramref permission seealso summary typeparam typeparamref value "
, // C# Attribute
"type name cref "
,
NULL
};

// XML Schema
static NP2_KEYWORDS kwXmlSchema = {
"xs xsd schema simpleType complexType complexContent simpleContent extension restriction element unique key keyref "
"selector field group all choice sequence any anyAttribute attribute attributeGroup include import redefine notation "
"annotation appinfo documentation "
//
"atomic enumeration list union pattern whiteSpace length minLength maxLength totalDigits fractionDigits "
"minInclusive maxInclusive minExclusive maxExclusive "
, // attribute
"ref targetNamespace finalDefault blockDefault elementFormDefault attributeFormDefault "
"abstract final block mixed base minOccurs maxOccurs nillable nil substitutionGroup default fixed form refer "
"xpath namespace processContents use schemaLocation public system source itemType memberTypes "
, // value
"anyType anySimpleType duration dateTime time date gYear gYearMonth gMonth gMonthDay gDay "
"boolean base64Binary hexBinary float double anyURI QName NOTATION "
"decimal integer long int short byte nonPositiveInteger negativeInteger nonNegativeInteger positiveInteger "
"unsignedLong unsignedInt unsignedShort unsignedByte "
"string normalizedString token language NMTOKEN NMTOKENS Name NCName ID IDREF IDREFS ENTITY ENTITIES "
"equal ordered bounded cardinality numeric " " preserve replace collapse "
"unqualified qualified strict skip lax prohibited optional required "
//"blockSet complexDerivationSet XPathExpr URIref "
, NULL
};

// XML Stylesheet
static NP2_KEYWORDS kwXmlStylesheet = {
"xsl stylesheet transform attribute-set character-map output-character decimal-format import import-schema include "
"key namespace-alias output param preserve-space strip-space template variable "
// instruction
"analyze-string apply-imports apply-templates attribute call-template choose when comment copy copy-of document element "
"fallback for-each for-each-group function if matching-substring message namespace next-match non-matching-substring "
"number otherwise perform-sort processing-instruction result-document sequence sort text value-of with-param "
, // attribute
"extension-element-prefixes exclude-result-prefixes xpath-default-namespace default-validation default-collation "
"input-type-annotations use-attribute-sets use-character-maps character string decimal-separator grouping-separator "
"infinity minus-sign NaN percent per-mille zero-digit digit pattern-separator href namespace schema-location match  "
"collation stylesheet-prefix result-prefix method byte-order-mark cdata-section-elements doctype-public doctype-system "
"escape-uri-attributes include-content-type indent media-type normalization-form omit-xml-declaration undeclare-prefixes "
"select as required tunnel elements priority regex flags mode separator test copy-namespaces inherit-namespaces "
"group-by group-adjacent group-starting-with group-ending-with override terminate level count from format  letter-value "
"ordinal  grouping-size output-version order stable case-order data-type disable-output-escaping "
, // value
"preserve strip unspecified default omit lax single multiple any alphabetic traditional ascending descending "
"upper-first lower-first text number xml html xhtml NFC NFD NFKC NFKD fully-normalized none "
, NULL
};

// Ant Build
static NP2_KEYWORDS kwAntBuild = {
// task or type
"project description property target tstamp " "include exclude patternset fileset path pathelement "
"echo mkdir copy delete move zip " "javac java javadoc javah jar "
, // attribute
"default basedir " "depends if unless " "id dir includes includesfile excludes excludesfile "
"value location refid resource file url environment classpath classpathref prefix relative "
"message " "tofile todir overwrite force includeEmptyDirs " "srcdir destdir   "
, // value
NULL, NULL
};

// Maven POM
static NP2_KEYWORDS kwMavenPOM = {
"project parent modelVersion groupId artifactId packaging name version description prerequisites issueManagement ciManagement "
"inceptionYear mailingLists mailingList developers developer contributors contributor licenses license scm organization build "
"profiles profile pluginManagement plugins plugin modules module repositories repository pluginRepositories pluginRepository "
"dependencies dependency reports reporting dependencyManagement distributionManagement properties "
"relativePath maven system notifiers notifier type sendOnError sendOnFailure sendOnSuccess sendOnWarning address configuration "
"subscribe unsubscribe post archive otherArchives otherArchive id email organizationUrl roles role timezone distribution comments "
"connection developerConnection tag sourceDirectory scriptSourceDirectory testSourceDirectory outputDirectory testOutputDirectory "
"defaultGoal extensions extension resources resource targetPath filtering directory includes include excludes exclude "
"testResources testResource finalName filters filter activation activeByDefault jdk os family arch property value "
"file missing exists executions execution phase goals goal inherited releases enabled updatePolicy checksumPolicy snapshots "
"layout classifier scope systemPath exclusions exclusion excludeDefaults reportSets reportSet report uniqueVersion "
"snapshotRepository downloadUrl relocation message status "
, NULL,
"jar HEAD"
, NULL
};

// Maven Settings
static NP2_KEYWORDS kwMavenSettings = {
"settings localRepository interactiveMode usePluginRegistry offline proxies proxy servers server "
"mirrors mirror profiles profile activeProfiles activeProfile pluginGroups pluginGroup "
"active protocol username password port host nonProxyHosts privateKey passphrase filePermissions "
"directoryPermissions configuration mirrorOf url layout mirrorOfLayouts activation properties "
"repositories pluginRepositories releases snapshots enabled updatePolicy checksumPolicy activeByDefault "
"jdk os property file missing exists family arch "
, NULL,
"http default"
, NULL
};

// Ivy Module
static NP2_KEYWORDS kwIvyModule = {
"ivy-module info extends license ivyauthor repository description configurations conf include "
"publications artifact dependencies dependency mapped exclude override conflict conflicts manager "
"configurations-conf global-exclude "
, // attribute
"organisation module branch revision status publication resolver namespace default location extendType "
"url pattern ivys artifacts homepage defaultconf defaultconfmapping confmappingoverride transitive "
"extends description deprecated visibility file ext conf org branchConstraint rev revConstraint "
"force changing mapped matcher "
, // value
"public private"
, NULL
};

// Ivy Settings
static NP2_KEYWORDS kwIvySettings = {
"ivysettings property properties settings include classpath typedef credentials lock-strategies "
"caches cache ttl latest-strategies version-matchers triggers ant-build ant-call log parsers conflict-managers outputters "
"namespaces namespace rule fromsystem src dest tosystem macrodef attribute resolvers resolver ivyrep ibiblio packager "
"filesystem url vfs ssh sftp jar chain dual obr updatesite ivy artifact modules module statuses status "
, // attribute
"override environment defaultResolver defaultLatestStrategy defaultConflictManager defaultBranch defaultResolveMode "
"circularDependencyStrategy validate useRemoteConfig httpRequestMethod defaultCache checkUpToDate cacheIvyPattern "
"cacheArtifactPattern url file classname host realm username passwd default defaultCacheDir resolutionCacheDir "
"repositoryCacheDir ivyPattern artifactPattern useOrigin lockStrategy basedir defaultTTL memorySize organisation "
"module revision matcher duration usedefaults pre-resolve pre-resolve-dependency post-resolve post-resolve-dependency "
"pre-download-artifact post-download-artifact pre-retrieve post-retrieve pre-retrieve-artifact post-retrieve-artifact "
"pre-publish-artifact post-publish-artifact event filter antfile target prefix message chainrules rev force checkModified "
"changingPattern changingMatcher alwaysCheckExactRevision namespace checkConsistency descriptor allownomd checksums latest "
"cache signer ref ivyroot artroot artPattern root pattern m2compatible usepoms useMavenMetadata buildRoot resourceCache "
"resourceURL restricted verbose quiet preserveBuildDirectories local transactional user userPassword keyFile keyFilePassword "
"port publishPermissions returnFirst dual repoXmlURL repoXmlFile requirementStrategy metadataTtl forceMetadataUpdate "
"resolver conflict-manager branch resolveMode integration "
, // value
"auto env default warn ignore error HEAD OPTIONS GET POST PUT DELETE TRACE CONNECT no-lock rtifact-lock "
"all latest-time latest-revision latest-compatible strict latest-cm compatible-cm regexp-cm "
, NULL
};

// PMD Ruleset
static NP2_KEYWORDS kwPMDRuleset = {
"ruleset description exclude-pattern include-pattern rule priority properties property exclude example "
, // attribute
"language minimumLanguageVersion maximumLanguageVersion since ref message "
"externalInfoUrl class dfa typeResolution deprecated delimiter min max "
, NULL, NULL
};

// Checkstyle Module
static NP2_KEYWORDS kwCheckstyle = {
"module property metadata message "
, // attribute
"default key"
, NULL, NULL
};

// Apache Config
// https://httpd.apache.org/docs/2.2/en/mod/directives.html
// https://httpd.apache.org/docs/2.4/en/mod/directives.html
static NP2_KEYWORDS kwApache = {
"AuthnProviderAlias AuthzProviderAlias Directory DirectoryMatch Else ElseIf Files FilesMatch If IfDefine IfModule IfVersion Limit LimitExcept Location LocationMatch Proxy ProxyMatch RequireAll RequireAny RequireNone VirtualHost "
, // Directive
"AcceptFilter AcceptMutex AcceptPathInfo AccessFileName Action AddAlt AddAltByEncoding AddAltByType AddCharset AddDefaultCharset AddDescription AddEncoding AddHandler AddIcon AddIconByEncoding AddIconByType AddInputFilter AddLanguage AddModuleInfo AddOutputFilter AddOutputFilterByType AddType "
"Alias AliasMatch Allow AllowCONNECT AllowEncodedSlashes AllowMethods AllowOverride AllowOverrideList Anonymous Anonymous_LogEmail Anonymous_MustGiveEmail Anonymous_NoUserID Anonymous_VerifyEmail AsyncRequestWorkerFactor "
"AuthBasicAuthoritative AuthBasicProvider AuthDBDUserPWQuery AuthDBDUserRealmQuery AuthDBMGroupFile AuthDBMType AuthDBMUserFile AuthDefaultAuthoritative AuthDigestAlgorithm AuthDigestDomain AuthDigestNcCheck AuthDigestNonceFormat AuthDigestNonceLifetime AuthDigestProvider AuthDigestQop AuthDigestShmemSize AuthFormAuthoritative AuthFormBody AuthFormDisableNoStore AuthFormFakeBasicAuth AuthFormLocation AuthFormLoginRequiredLocation AuthFormLoginSuccessLocation AuthFormLogoutLocation AuthFormMethod AuthFormMimetype AuthFormPassword AuthFormProvider AuthFormSitePassphrase AuthFormSize AuthFormUsername AuthGroupFile "
"AuthLDAPAuthorizePrefix AuthLDAPBindAuthoritative AuthLDAPBindDN AuthLDAPBindPassword AuthLDAPCharsetConfig AuthLDAPCompareAsUser AuthLDAPCompareDNOnServer AuthLDAPDereferenceAliases AuthLDAPGroupAttribute AuthLDAPGroupAttributeIsDN AuthLDAPInitialBindAsUser AuthLDAPInitialBindPattern AuthLDAPMaxSubGroupDepth AuthLDAPRemoteUserAttribute AuthLDAPRemoteUserIsDN AuthLDAPSearchAsUser AuthLDAPSubGroupAttribute AuthLDAPSubGroupClass AuthLDAPUrl "
"AuthMerging AuthName AuthnCacheContext AuthnCacheEnable AuthnCacheProvideFor AuthnCacheSOCache AuthnCacheTimeout AuthType AuthUserFile AuthzDBDLoginToReferer AuthzDBDQuery AuthzDBDRedirectQuery AuthzDBMAuthoritative AuthzDBMType AuthzDefaultAuthoritative AuthzGroupFileAuthoritative AuthzLDAPAuthoritative AuthzOwnerAuthoritative AuthzSendForbiddenOnFailure AuthzUserAuthoritative "
"BalancerGrowth BalancerMember BrowserMatch BrowserMatchNoCase BufferedLogs BufferSize "
"CacheDefaultExpire CacheDetailHeader CacheDirLength CacheDirLevels CacheDisable CacheEnable CacheFile CacheHeader CacheIgnoreCacheControl CacheIgnoreHeaders CacheIgnoreNoLastMod CacheIgnoreQueryString CacheIgnoreURLSessionIdentifiers CacheKeyBaseURL CacheLastModifiedFactor CacheLock CacheLockMaxAge CacheLockPath CacheMaxExpire CacheMaxFileSize CacheMinExpire CacheMinFileSize CacheNegotiatedDocs CacheQuickHandler CacheReadSize CacheReadTime CacheRoot CacheStaleOnError CacheStoreExpired CacheStoreNoStore CacheStorePrivate "
"CGIMapExtension CharsetDefault CharsetOptions CharsetSourceEnc CheckCaseOnly CheckSpelling ChrootDir ContentDigest CookieDomain CookieExpires CookieLog CookieName CookieStyle CookieTracking CoreDumpDirectory CustomLog "
"Dav DavDepthInfinity DavGenericLockDB DavLockDB DavMinTimeout DBDExptime DBDInitSQL DBDKeep DBDMax DBDMin DBDParams DBDPersist DBDPrepareSQL DBDriver DefaultIcon DefaultLanguage DefaultRuntimeDir DefaultType Define DeflateBufferSize DeflateCompressionLevel DeflateFilterNote DeflateMemLevel DeflateWindowSize Deny DirectoryIndex DirectoryIndexRedirect DirectorySlash DocumentRoot DTracePrivileges DumpIOInput DumpIOLogLevel DumpIOOutput "
"EnableExceptionHook EnableMMAP EnableSendfile Error ErrorDocument ErrorLog ErrorLogFormat Example ExpiresActive ExpiresByType ExpiresDefault ExtendedStatus ExtFilterDefine ExtFilterOptions "
"FallbackResource FileETag FilterChain FilterDeclare FilterProtocol FilterProvider FilterTrace ForceLanguagePriority ForceType ForensicLog GprofDir GracefulShutdownTimeout Group Header HeaderName HeartbeatAddress HeartbeatListen HeartbeatMaxServers HeartbeatStorage HostnameLookups "
"IdentityCheck IdentityCheckTimeout ImapBase ImapDefault ImapMenu Include IncludeOptional IndexHeadInsert IndexIgnore IndexIgnoreReset IndexOptions IndexOrderDefault IndexStyleSheet InputSed ISAPIAppendLogToErrors ISAPIAppendLogToQuery ISAPICacheFile ISAPIFakeAsync ISAPILogNotSupported ISAPIReadAheadBuffer KeepAlive KeepAliveTimeout KeptBodySize "
"LanguagePriority LDAPCacheEntries LDAPCacheTTL LDAPConnectionPoolTTL LDAPConnectionTimeout LDAPLibraryDebug LDAPOpCacheEntries LDAPOpCacheTTL LDAPReferralHopLimit LDAPReferrals LDAPRetries LDAPRetryDelay LDAPSharedCacheFile LDAPSharedCacheSize LDAPTimeout LDAPTrustedClientCert LDAPTrustedGlobalCert LDAPTrustedMode LDAPVerifyServerCert "
"LimitInternalRecursion LimitRequestBody LimitRequestFields LimitRequestFieldSize LimitRequestLine LimitXMLRequestBody Listen ListenBackLog LoadFile LoadModule LockFile LogFormat LogLevel LogMessage "
"LuaAuthzProvider LuaHookAccessChecker LuaHookAuthChecker LuaHookCheckUserID LuaHookFixups LuaHookInsertFilter LuaHookMapToStorage LuaHookTranslateName LuaHookTypeChecker LuaInherit LuaPackageCPath LuaPackagePath LuaQuickHandler LuaRoot LuaScope "
"MaxClients MaxConnectionsPerChild MaxKeepAliveRequests MaxMemFree MaxRangeOverlaps MaxRangeReversals MaxRanges MaxRequestsPerChild MaxRequestsPerThread MaxRequestWorkers MaxSpareServers MaxSpareThreads MaxThreads MCacheMaxObjectCount MCacheMaxObjectSize MCacheMaxStreamingBuffer MCacheMinObjectSize MCacheRemovalAlgorithm MCacheSize MetaDir MetaFiles MetaSuffix MimeMagicFile MinSpareServers MinSpareThreads MMapFile ModemStandard ModMimeUsePathInfo MultiviewsMatch Mutex NameVirtualHost NoProxy NWSSLTrustedCerts NWSSLUpgradeable Options Order OutputSed "
"PassEnv PidFile PrivilegesMode Protocol ProtocolEcho ProxyAddHeaders ProxyBadHeader ProxyBlock ProxyDomain ProxyErrorOverride ProxyExpressDBMFile ProxyExpressDBMType ProxyExpressEnable ProxyFtpDirCharset ProxyFtpEscapeWildcards ProxyFtpListOnWildcard ProxyHTMLBufSize ProxyHTMLCharsetOut ProxyHTMLDocType ProxyHTMLEnable ProxyHTMLEvents ProxyHTMLExtended ProxyHTMLFixups ProxyHTMLInterp ProxyHTMLLinks ProxyHTMLMeta ProxyHTMLStripComments ProxyHTMLURLMap ProxyIOBufferSize ProxyMaxForwards ProxyPass ProxyPassInterpolateEnv ProxyPassMatch ProxyPassReverse ProxyPassReverseCookieDomain ProxyPassReverseCookiePath ProxyPreserveHost ProxyReceiveBufferSize ProxyRemote ProxyRemoteMatch ProxyRequests ProxySCGIInternalRedirect ProxySCGISendfile ProxySet ProxySourceAddress ProxyStatus ProxyTimeout ProxyVia "
"ReadmeName ReceiveBufferSize Redirect RedirectMatch RedirectPermanent RedirectTemp ReflectorHeader RemoteIPHeader RemoteIPInternalProxy RemoteIPInternalProxyList RemoteIPProxiesHeader RemoteIPTrustedProxy RemoteIPTrustedProxyList RemoveCharset RemoveEncoding RemoveHandler RemoveInputFilter RemoveLanguage RemoveOutputFilter RemoveType RequestHeader RequestReadTimeout Require RewriteBase RewriteCond RewriteEngine RewriteLock RewriteLog RewriteLogLevel RewriteMap RewriteOptions RewriteRule RLimitCPU RLimitMEM RLimitNPROC "
"Satisfy ScoreBoardFile Script ScriptAlias ScriptAliasMatch ScriptInterpreterSource ScriptLog ScriptLogBuffer ScriptLogLength ScriptSock SecureListen SeeRequestTail SendBufferSize ServerAdmin ServerAlias ServerLimit ServerName ServerPath ServerRoot ServerSignature ServerTokens Session SessionCookieName SessionCookieName2 SessionCookieRemove SessionCryptoCipher SessionCryptoDriver SessionCryptoPassphrase SessionCryptoPassphraseFile SessionDBDCookieName SessionDBDCookieName2 SessionDBDCookieRemove SessionDBDDeleteLabel SessionDBDInsertLabel SessionDBDPerUser SessionDBDSelectLabel SessionDBDUpdateLabel SessionEnv SessionExclude SessionHeader SessionInclude SessionMaxAge SetEnv SetEnvIf SetEnvIfExpr SetEnvIfNoCase SetHandler SetInputFilter SetOutputFilter SSIEnableAccess SSIEndTag SSIErrorMsg SSIETag SSILastModified "
"SSILegacyExprParser SSIStartTag SSITimeFormat SSIUndefinedEcho SSLCACertificateFile SSLCACertificatePath SSLCADNRequestFile SSLCADNRequestPath SSLCARevocationCheck SSLCARevocationFile SSLCARevocationPath SSLCertificateChainFile SSLCertificateFile SSLCertificateKeyFile SSLCipherSuite SSLCompression SSLCryptoDevice SSLEngine SSLFIPS SSLHonorCipherOrder SSLInsecureRenegotiation SSLMutex SSLOCSPDefaultResponder SSLOCSPEnable SSLOCSPOverrideResponder SSLOCSPResponderTimeout SSLOCSPResponseMaxAge SSLOCSPResponseTimeSkew SSLOptions SSLPassPhraseDialog SSLProtocol "
"SSLProxyCACertificateFile SSLProxyCACertificatePath SSLProxyCARevocationCheck SSLProxyCARevocationFile SSLProxyCARevocationPath SSLProxyCheckPeerCN SSLProxyCheckPeerExpire SSLProxyCipherSuite SSLProxyEngine SSLProxyMachineCertificateChainFile SSLProxyMachineCertificateFile SSLProxyMachineCertificatePath SSLProxyProtocol SSLProxyVerify SSLProxyVerifyDepth "
"SSLRandomSeed SSLRenegBufferSize SSLRequire SSLRequireSSL SSLSessionCache SSLSessionCacheTimeout SSLSessionTicketKeyFile SSLStaplingCache SSLStaplingErrorCacheTimeout SSLStaplingFakeTryLater SSLStaplingForceURL SSLStaplingResponderTimeout SSLStaplingResponseMaxAge SSLStaplingResponseTimeSkew SSLStaplingReturnResponderErrors SSLStaplingStandardCacheTimeout SSLStrictSNIVHostCheck SSLUserName SSLUseStapling SSLVerifyClient SSLVerifyDepth StartServers StartThreads Substitute Suexec SuexecUserGroup "
"ThreadLimit ThreadsPerChild ThreadStackSize TimeOut TraceEnable TransferLog TypesConfig UnDefine UnsetEnv UseCanonicalName UseCanonicalPhysicalPort User UserDir VHostCGIMode VHostCGIPrivs VHostGroup VHostPrivs VHostSecure VHostUser VirtualDocumentRoot VirtualDocumentRootIP VirtualScriptAlias VirtualScriptAliasIP WatchdogInterval Win32DisableAcceptEx XBitHack xml2EncAlias xml2EncDefault xml2StartParse"
, // Valves
NULL
, NULL
};

// Tomcat Config
static NP2_KEYWORDS kwTomcat = {
"Server Service GlobalNamingResources Resource ResourceLink Transaction Executor Connector Context Parameter Environment Engine Realm "
"Host Valve JarScanner Listener Loader Manager Filter "
"Cluster Channel Membership Sender Transport Receiver Interceptor Member StaticMember Deployer ClusterListener "
, // attribute
"port shutdown address className "
// Resource
"description override auth scope closeMethod singleton extraResourcePaths global factory username password "
// Executor
"threadPriority daemon namePrefix maxThreads minSpareThreads maxIdleTime maxQueueSize prestartminSpareThreads threadRenewalDelay "
// Connector
"protocol scheme allowTrace asyncTimeout enableLookups maxHeaderCount maxParameterCount maxPostSize maxSavePostSize parseBodyMethods "
"proxyName proxyPort redirectPort secure URIEncoding useBodyEncodingForURI useIPVHosts xpoweredBy "
"acceptCount acceptorThreadCount acceptorThreadPriority bindOnInit compressableMimeType compression compressionMinSize connectionLinger "
"connectionTimeout connectionUploadTimeout disableUploadTimeout executor keepAliveTimeout maxConnections maxHttpHeaderSize maxKeepAliveRequests "
"maxTrailerSize noCompressionUserAgents processorCache restrictedUserAgents server socketBuffer SSLEnabled tcpNoDelay "
"clientCertProvider packetSize requiredSecret tomcatAuthentication socket selectorPool rxBufSize txBufSize soKeepAlive ooBInline "
"soReuseAddress soLingerOn soLingerTime soTimeout performanceConnectionTime performanceLatency performanceBandwidth unlockTimeout "
"disableKeepAlivePercentage pollerThreadCount pollerThreadPriority selectorTimeout useComet useSendfile "
"directBuffer appReadBufSize appWriteBufSize bufferPool bufferPoolSize keyCache eventCache maxSelectors maxSpareSelectors "
"oomParachute deferAccept pollerSize pollTime sendfileSize sendfileThreadCount "
"algorithm allowUnsafeLegacyRenegotiation ciphers clientAuth crlFile keyAlias keyPass keystoreFile keystorePass keystoreProvider "
"keystoreType sessionCacheSize sessionTimeout sslEnabledProtocols sslImplementationName sslProtocol trustManagerClassName trustMaxCertLength "
"truststoreAlgorithm truststoreFile truststorePass truststoreProvider "
"SSLCACertificatePath SSLCARevocationFile SSLCARevocationPath SSLCertificateChainFile SSLCACertificateFile SSLCertificateFile "
"SSLCertificateKeyFile SSLCipherSuite SSLHonorCipherOrder SSLPassword SSLProtocol SSLVerifyClient SSLVerifyDepth "
// Context
"docBase allowCasualMultipartParsing backgroundProcessorDelay cookies crossContext fireRequestListenersOnForwards logEffectiveWebXml path "
"preemptiveAuthentication privileged reloadable resourceOnlyServlets sendRedirectBody sessionCookieDomain sessionCookieName sessionCookiePath "
"sessionCookiePathUsesTrailingSlash swallowAbortedUploads swallowOutput tldNamespaceAware tldValidation useHttpOnly wrapperClass "
"xmlNamespaceAware xmlValidation "
"addWebinfClassesResources aliases allowLinking antiJARLocking antiResourceLocking cacheMaxSize cacheObjectMaxSize cacheTTL cachingAllowed "
"clearReferencesHttpClientKeepAliveThread clearReferencesStatic clearReferencesStopThreads clearReferencesStopTimerThreads "
"jndiExceptionOnFailedWrite processTlds renewThreadsWhenStoppingContext unloadDelay unpackWAR useNaming workDir "
// Engine/Host
"defaultHost jvmRoute startStopThreads " "createDirs autoDeploy deployIgnore deployOnStartup "
// Listener
"SSLEngine SSLRandomSeed FIPSMode appContextProtection AWTThreadProtection classesToInitialize driverManagerProtection gcDaemonProtection "
"java2DDisposerProtection ldapPoolProtection securityLoginConfigurationProtection securityPolicyProtection tokenPollerProtection "
"urlCacheProtection xmlParsingProtection checkedOsUsers minimumUmask directoryName userClass homeBase rmiRegistryPortPlatform "
"rmiServerPortPlatform useLocalPorts "
// Loader
"delegate loaderClass searchExternalFirst virtualClasspath searchVirtualFirst " "scanClassPath scanAllFiles scanAllDirectories "
// Manager
"distributable maxActiveSessions maxInactiveInterval sessionIdLength pathname processExpiresFrequency secureRandomClass secureRandomProvider "
"secureRandomAlgorithm maxIdleBackup maxIdleSwap minIdleSwap saveOnRestart checkInterval directory connectionURL dataSourceName driverName "
"sessionAppCol sessionDataCol sessionIdCol sessionLastAccessedCol sessionMaxInactiveCol sessionTable sessionValidCol "
// Filter
"entryPoints nonceCacheSize randomClass ExpiresExcludedResponseStatusCodes ExpiresByType ExpiresDefault allow deny denyStatus remoteIpHeader "
"internalProxies proxiesHeader requestAttributesEnabled trustedProxies protocolHeader portHeader protocolHeaderHttpsValue httpServerPort "
"httpsServerPort changeLocalPort ignore "
// Valve
"locale pattern prefix resolveHosts suffix rotatable conditionIf conditionUnless condition fileDateFormat buffered crawlerUserAgents "
"sessionInactiveInterval threshold requireReauthentication cookieDomain alwaysUseSession cache changeSessionIdOnAuthentication "
"disableProxyCaching securePagesWithPragma key nonceValidity opaque validateUri landingPage loginConfigName storeDelegatedCredential "
// Cluster
"channelSendOptions channelStartOptions heartbeatBackgroundEnabled doClusterLog clusterLogName "
"notifyListenersOnReplication expireSessionsOnShutdown sessionAttributeFilter notifySessionListenersOnReplication "
"notifyContainerListenersOnReplication stateTransferTimeout mapSendOptions rpcTimeout frequency dropTime bind ttl domain "
"recoveryEnabled recoveryCounter recoverySleepTime udpRxBufSize udpTxBufSize keepAliveCount keepAliveTime timeout maxRetryAttempts "
"soTrafficClass throwOnFailedAck poolSize direct autoBind securePort udpPort minThreads alwaysSend interval host uniqueId "
, // value
"localhost HTTP AJP http https "
// values for className
"org apache catalina core resources realm valves authenticator session filters loader startup users mbeans tribes group interceptors "
"StandardServer StandardService StandardEngine StandardHost StandardThreadExecutor StandardJarScanner "
"StandardContext FileDirContext VirtualDirContext " "WebappLoader VirtualWebappLoader "
"LifecycleListener JasperListener AprLifecycleListener GlobalResourcesLifecycleListener JreMemoryLeakPreventionListener "
"SecurityListener ThreadLocalLeakPreventionListener UserConfig JmxRemoteLifecycleListener "
"ClusterSessionListener JvmRouteSessionIDBinderListener "
"StandardManager PersistentManager FileStore JDBCStore " "ClusterManager DeltaManager BackupManager"
"AddDefaultCharsetFilter CsrfPreventionFilter ExpiresFilter RemoteAddrFilter RemoteHostFilter RemoteIpFilter "
"RequestDumperFilter SetCharacterEncodingFilter WebdavFixFilter FailedRequestFilter "
"JDBCRealm DataSourceRealm JNDIRealm UserDatabaseRealm MemoryRealm JAASRealm CombinedRealm LockOutRealm "
"AccessLogValve ExtendedAccessLogValve RemoteAddrValve RemoteHostValve RemoteIpValve CrawlerSessionManagerValve StuckThreadDetectionValve "
"SingleSignOn BasicAuthenticator DigestAuthenticator FormAuthenticator SSLAuthenticator SpnegoAuthenticator "
"DomainFilterInterceptor MessageDispatch15Interceptor MessageDispatchInterceptor TcpFailureDetector ThroughputInterceptor "
, NULL
};

// Java web.xml
static NP2_KEYWORDS kwJavaWebConfig = {
"web-app distributable context-param filter filter-mapping filter-name filter-class listener listener-class "
"servlet servlet-mapping servlet-name servlet-class session-config session-timeout mime-mapping mime-type "
" welcome-file-list welcome-file error-page error-code form-error-page jsp-config jsp-file jsp-property-group "
"secure security-constraint security-role security-role-ref login-config login-timeout form-login-config form-login-page "
"locale locale-encoding-mapping-list locale-encoding-mapping encoding page-encoding description location include-prelude include-coda "
"message-destination "
" module-name absolute-ordering ordering   exception-type  "
"  async-supported init-param url-pattern  dispatcher   "
"auth-method realm-name  extension display-name web-resource-collection auth-constraint user-data-constraint "
"  load-on-startup enabled run-as  multipart-config  cookie-config tracking-mode "
"domain path comment http-only  max-age transport-guarantee web-resource-name http-method http-method-omission  "
"   others after before max-file-size max-request-size file-size-threshold "
"taglib  el-ignored  scripting-invalid is-xml  "
"deferred-syntax-allowed-as-literal trim-directive-whitespaces default-content-type buffer error-on-undeclared-namespace "
"taglib-uri taglib-location service-ref-name service-interface service-ref-type wsdl-file jaxrpc-mapping-file service-qname "
"port-component-ref handler handler-chains service-endpoint-interface enable-mtom mtom-threshold addressing respect-binding "
"port-component-link handler-chain service-name-pattern port-name-pattern protocol-bindings required responses handler-name "
"handler-class soap-header soap-role port-name service-ref "
"icon env-entry ejb-ref ejb-local-ref resource-ref resource-env-ref message-destination-ref persistence-context-ref persistence-unit-ref "
"post-construct pre-destroy data-source lookup-name mapped-name injection-target class-name server-name port-number database-name url "
"user password property  transactional isolation-level initial-pool-size max-pool-size min-pool-size max-idle-time "
"max-statements ejb-ref-name ejb-ref-type local-home local ejb-link ejb-ref-name ejb-ref-type home remote ejb-link env-entry-name "
"env-entry-type env-entry-value small-icon large-icon injection-target-class injection-target-name lifecycle-callback-class "
"lifecycle-callback-method  param-name param-value persistence-context-ref-name persistence-unit-name "
"persistence-context-type persistence-property persistence-unit-ref-name resource-env-ref-name resource-env-ref-type res-ref-name "
"res-type res-auth res-sharing-scope role-name role-link message-destination-name mapped-name lookup-name message-destination-ref-name "
"message-destination-type message-destination-usage message-destination-link "
,
" metadata-complete  "
, NULL
};

// Struts config and validators
static NP2_KEYWORDS kwStruts = {
"struts package result-types result-type param interceptors interceptor interceptor-stack interceptor-ref "
"default-interceptor-ref default-action-ref default-class-ref global-results result global-exception-mappings "
"exception-mapping action allowed-methods include bean constant unknown-handler-stack unknown-handler-ref "
"xwork parameters parameter external-ref struts-config "
"validators field field-validator message validator "
, // attribute
"order extends namespace abstract strict-method-invocation externalReferenceResolver "
"class default exception method converter file scope static optional " "required short-circuit key "
, // value
NULL, NULL
};

// hibernate config
static NP2_KEYWORDS kwHibConfig = {
"hibernate-configuration session-factory property mapping class-cache collection-cache event listener security grant "
"jcs-class-cache jcs-collection-cache "
, // attribute
"resource file jar package class region include usage collection context role entity-name actions "
, // value
"all non-lazy read-only read-write nonstrict-read-write transactional "
"auto-flushmerge create create-onflush delete dirty-check evict flush flush-entity load load-collection "
"lock refresh replicate save-update save update pre-load pre-update pre-insert pre-delete pre-collection-recreate "
"pre-collection-remove pre-collection-update post-load post-update post-insert post-delete post-collection-recreate "
"post-collection-remove post-collection-update post-commit-update post-commit-insert post-commit-delete "
, NULL
};

// hibernate mapping
static NP2_KEYWORDS kwHibMapping = {
"hibernate-mapping meta identifier-generator typedef import class tuplizer filter-def filter-param filter fetch-profile fetch join "
"natural-id id composite-id discriminator version timestamp subclass joined-subclass union-subclass property type many-to-one "
"one-to-one key-property key-many-to-one any meta-value component dynamic-component properties parent map set bag idbag list "
"array primitive-array element one-to-many many-to-many composite-element key list-index map-key index map-key-many-to-many "
"index-many-to-many composite-map-key composite-index many-to-any collection-id generator param column formula subselect cache "
"comment loader query sql-query query-param resultset return return-property return-column return-discriminator return-join "
"load-collection return-scalar synchronize sql-insert sql-update sql-delete sql-delete-all database-object definition create "
"drop dialect-scope "
, // attribute
"schema catalog default-cascade default-access default-lazy auto-import package attribute inherit class rename entity-name "
"proxy table subselect discriminator-value mutable abstract polymorphism where persister dynamic-update dynamic-insert batch-size "
"select-before-update optimistic-lock check rowid node entity-mode condition entity association style fetch inverse optional "
"column type length unsaved-value mapped formula force insert source generated extends precision scale unique unique-key index "
"update outer-join foreign-key property-ref not-found constrained embed-xml id-type meta-type sort order-by collection-type "
"element-class on-delete base sql-type default read write usage region include query-ref flush-mode cacheable cache-region "
"fetch-size timeout cache-mode read-only comment resultset-ref callable alias lock-mode role "
, // value
"none property implicit explicit dirty all pojo dom4j dynamic-map join select undefined any null negative vm db never auto "
"no-proxy exception ignore extra unsorted noaction read-write nonstrict-read-write transactional non-lazy always get normal "
"put refresh upgrade-nowait upgrade rowcount param "
, NULL
};

// Spring Beans
static NP2_KEYWORDS kwSpringBeans = {
"beans description import alias bean meta constructor-arg ref idref value null array list set map entry key props "
"property qualifier lookup-method replaced-method arg-type "
, // attribute
"profile default-lazy-init default-merge default-autowire default-autowire-candidates default-init-method default-destroy-method "
"resource alias class parent scope abstract lazy-init autowire depends-on autowire-candidate primary "
"init-method destroy-method factory-method factory-bean "
"key index ref local merge value-type key-type key-ref value-ref replacer match "
, // value
"default byName byType constructor "
, NULL
};

// JBoss Config

// ASP.NET Web Config

// ResX Schema

// WPF XAML

// Apple Property List
static NP2_KEYWORDS kwPList = {
"plist array data date dict key real integer string "
, NULL, NULL, NULL
};

// Android Manifest
static NP2_KEYWORDS kwAndroidManifest = {
"manifest application activity activity-alias service provider receiver uses-library "
"intent-filter category action data meta-data grant-uri-permission path-permission  "
"compatible-screens screen instrumentation permission permission-group permission-tree "
"supports-gl-texture supports-screens uses-configuration uses-feature uses-permission uses-sdk "
, // attribute
"android package sharedUserId sharedUserLabel versionCode versionName installLocation "
"allowTaskReparenting backupAgent debuggable description enabled hasCode hardwareAccelerated icon killAfterRestore "
"largeHeap label logo manageSpaceActivity name permission persistent process restoreAnyVersion taskAffinity theme uiOptions "
"alwaysRetainTaskState clearTaskOnLaunch configChanges excludeFromRecents exported finishOnTaskLaunch launchMode "
"multiprocess noHistory parentActivityName screenOrientation stateNotNeeded windowSoftInputMode "
"priority host mimeType path pathPrefix pathPattern port scheme resource "
"targetActivity authorities grantUriPermissions initOrder readPermission syncable writePermission isolatedProcess required "
"screenSize screenDensity functionalTest handleProfiling targetPackage permissionGroup protectionLevel resizeable "
"smallScreens normalScreens largeScreens xlargeScreens anyDensity requiresSmallestWidthDp compatibleWidthLimitDp largestWidthLimitDp "
"reqFiveWayNav reqHardKeyboard reqKeyboardType reqNavigation reqTouchScreen glEsVersion minSdkVersion targetSdkVersion maxSdkVersion "
, // value
"internalOnly auto preferExternal " "none splitActionBarWhenNarrow " "standard singleTop singleTask singleInstance "
"mcc mnc locale touchscreen keyboard keyboardHidden navigation screenLayout fontScale uiMode orientation screenSize smallestScreenSize "
"unspecified user behind landscape portrait reverseLandscape reversePortrait sensorLandscape sensorPortrait sensor fullSensor nosensor"
"stateUnspecified stateUnchanged stateHidden stateAlwaysHidden stateVisible stateAlwaysVisible adjustUnspecified adjustResize adjustPan "
"small normal large xlarge ldpi mdpi hdpi xhdpi " "dangerous signature signatureOrSystem "
"undefined nokeys qwerty twelvekey nonav dpad trackball wheel notouch stylus finger "
"GL_OES_compressed_ETC1_RGB8_texture GL_OES_compressed_paletted_texture GL_AMD_compressed_3DC_texture GL_AMD_compressed_ATC_texture "
"GL_EXT_texture_compression_latc GL_EXT_texture_compression_dxt1 GL_EXT_texture_compression_s3tc GL_IMG_texture_compression_pvrtc"
, NULL
};

// Android Layout


// SVG Document


NP2_KEYWORDS *np2_LexKeyword;

void Style_UpdateLexerKeywords(LPCEDITLEXER pLexNew) {
	np2_LexKeyword = NULL;
	// additional keywords
	switch (pLexNew->rid) {
	case NP2LEX_JAVA:	// Java Platform
	case NP2LEX_GROOVY:
	case NP2LEX_SCALA:
		np2_LexKeyword = &kwJavaDoc;
		return;
	case NP2LEX_PHP:
		np2_LexKeyword = &kwPHPDoc;
		return;
	case NP2LEX_CSHARP:	// .NET Platform
	case NP2LEX_FSHARP:
		//case NP2LEX_VB:
		np2_LexKeyword = &kwNETDoc;
		return;
	case NP2LEX_CPP:
		np2_LexKeyword = &kwDoxyDoc;
		return;
	}

	if (np2LexLangIndex == 0) {
		return;
	}
	switch (np2LexLangIndex) {
	case IDM_LANG_XSD:
		np2_LexKeyword = &kwXmlSchema;
		return;
	case IDM_LANG_XSLT:
		np2_LexKeyword = &kwXmlStylesheet;
		return;

	case IDM_LANG_ANT_BUILD:
		np2_LexKeyword = &kwAntBuild;
		return;
	case IDM_LANG_MAVEN_POM:
		np2_LexKeyword = &kwMavenPOM;
		return;
	case IDM_LANG_MAVEN_SETTINGS:
		np2_LexKeyword = &kwMavenSettings;
		return;
	case IDM_LANG_IVY_MODULE:
		np2_LexKeyword = &kwIvyModule;
		return;
	case IDM_LANG_IVY_SETTINGS:
		np2_LexKeyword = &kwIvySettings;
		return;
	case IDM_LANG_PMD_RULESET:
		np2_LexKeyword = &kwPMDRuleset;
		return;
	case IDM_LANG_CHECKSTYLE:
		np2_LexKeyword = &kwCheckstyle;
		return;

	case IDM_LANG_APACHE:
		np2_LexKeyword = &kwApache;
		return;
	case IDM_LANG_TOMCAT:
		np2_LexKeyword = &kwTomcat;
		return;
	case IDM_LANG_WEB_JAVA:
		np2_LexKeyword = &kwJavaWebConfig;
		return;
	case IDM_LANG_STRUTS:
		np2_LexKeyword = &kwStruts;
		return;
	case IDM_LANG_HIB_CFG:
		np2_LexKeyword = &kwHibConfig;
		return;
	case IDM_LANG_HIB_MAP:
		np2_LexKeyword = &kwHibMapping;
		return;
	case IDM_LANG_SPRING_BEANS:
		np2_LexKeyword = &kwSpringBeans;
		return;
	case IDM_LANG_JBOSS:
		return;

	//case IDM_LANG_WEB_NET:
	//	return;
	//case IDM_LANG_RESX:
	//	np2_LexKeyword = &kwXmlSchema;
	//	return;
	//case IDM_LANG_XAML:
	//	return;

	case IDM_LANG_PROPERTY_LIST:
		np2_LexKeyword = &kwPList;
		return;
	case IDM_LANG_ANDROID_MANIFEST:
		np2_LexKeyword = &kwAndroidManifest;
		return;
	case IDM_LANG_ANDROID_LAYOUT:
		return;
		//case IDM_LANG_SVG:
		//	return;
	}
}
