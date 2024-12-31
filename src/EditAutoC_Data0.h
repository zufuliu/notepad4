typedef const char * const NP2_KEYWORDS[4];

// Doxygen
static NP2_KEYWORDS kwDoxyDoc = {
"a addindex addtogroup anchor arg attention author authors b brief bug c callgraph callergraph category cite class code cond copybrief copydetails copydoc copyright date def defgroup deprecated details dir docbookonly dontinclude dot dotfile e else elseif em endcode endcond enddocbookonly enddot endhtmlonly endif endinternal endlatexonly endlink endmanonly endmsc endrtfonly endsecreflist endverbatim endxmlonly enum example exception extends file fn headerfile hideinitializer htmlinclude htmlonly idlexcept if ifnot image implements include includelineno ingroup internal invariant interface latexonly li line link mainpage manonly memberof msc mscfile n name namespace nosubgrouping note overload p package page par paragraph param post pre private privatesection property protected protectedsection protocol public publicsection pure ref refitem related relates relatedalso relatesalso remark remarks result return returns retval rtfonly sa secreflist section see short showinitializer since skip skipline snippet struct subpage subsection subsubsection tableofcontents test throw throws todo tparam typedef union until var verbatim verbinclude version vhdlflow warning weakgroup xmlonly xrefitem "
, // C#
"inheritdoc list description item term listheader para paramref permission seealso summary typeparam typeparamref value "
, // C# Attribute
"type name cref "
,
nullptr
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
"boolean base64Binary hexBinary float double anyURI QName "
"decimal integer long int short byte nonPositiveInteger negativeInteger nonNegativeInteger positiveInteger "
"unsignedLong unsignedInt unsignedShort unsignedByte "
"string normalizedString token language Name NCName "
"equal ordered bounded cardinality numeric preserve replace collapse "
"unqualified qualified strict skip lax prohibited optional required "
//"blockSet complexDerivationSet XPathExpr URIref "
, nullptr
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
"infinity minus-sign NaN percent per-mille zero-digit digit pattern-separator href namespace schema-location match "
"collation stylesheet-prefix result-prefix method byte-order-mark cdata-section-elements doctype-public doctype-system "
"escape-uri-attributes include-content-type indent media-type normalization-form omit-xml-declaration undeclare-prefixes "
"select as required tunnel elements priority regex flags mode separator test copy-namespaces inherit-namespaces "
"group-by group-adjacent group-starting-with group-ending-with override terminate level count from format letter-value "
"ordinal grouping-size output-version order stable case-order data-type disable-output-escaping "
, // value
"preserve strip unspecified default omit lax single multiple any alphabetic traditional ascending descending "
"upper-first lower-first text number xml html xhtml NFC NFD NFKC NFKD fully-normalized none "
, nullptr
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
nullptr
, nullptr
};

// Apple Property List
static NP2_KEYWORDS kwPList = {
"plist array data date dict key real integer string "
, nullptr, nullptr, nullptr
};


// SVG Document


static NP2_KEYWORDS *np2_LexKeyword;

static inline void UpdateLexerExtraKeywords() noexcept {
	if (np2LexLangIndex == 0) {
		return;
	}
	switch (np2LexLangIndex) {
	case IDM_LEXER_XSD:
		np2_LexKeyword = &kwXmlSchema;
		break;
	case IDM_LEXER_XSLT:
		np2_LexKeyword = &kwXmlStylesheet;
		break;

	case IDM_LEXER_APACHE:
		np2_LexKeyword = &kwApache;
		break;
	case IDM_LEXER_PROPERTY_LIST:
		np2_LexKeyword = &kwPList;
		break;
		//case IDM_LEXER_SVG:
		//	break;
	}
}
