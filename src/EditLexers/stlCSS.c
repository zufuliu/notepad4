#include "EditLexer.h"
#include "EditStyle.h"

// https://en.wikipedia.org/wiki/Cascading_Style_Sheets
// https://www.w3.org/Style/CSS/

static KEYWORDLIST Keywords_CSS = {{
"^-moz- ^-ms- ^-o- ^-webkit- alignment-adjust alignment-baseline animation animation-delay "
"animation-direction animation-duration animation-iteration-count animation-name "
"animation-play-state animation-timing-function appearance ascent azimuth backface-visibility "
"background background-attachment background-break background-clip background-color "
"background-image background-origin background-position background-repeat background-size "
"baseline baseline-shift bbox binding bleed bookmark-label bookmark-level bookmark-state "
"bookmark-target border border-bottom border-bottom-color border-bottom-left-radius "
"border-bottom-right-radius border-bottom-style border-bottom-width border-collapse border-color "
"border-image border-image-outset border-image-repeat border-image-slice border-image-source "
"border-image-width border-left border-left-color border-left-style border-left-width "
"border-length border-radius border-right border-right-color border-right-style "
"border-right-width border-spacing border-style border-top border-top-color "
"border-top-left-radius border-top-right-radius border-top-style border-top-width border-width "
"bottom box-align box-decoration-break box-direction box-flex box-flex-group box-lines "
"box-ordinal-group box-orient box-pack box-shadow box-sizing break-after break-before "
"break-inside cap-height caption-side centerline change-bar change-bar-class change-bar-offset "
"change-bar-side clear clip color color-profile column-count column-fill column-gap column-rule "
"column-rule-color column-rule-style column-rule-width columns column-span column-width content "
"counter-increment counter-reset crop cue cue-after cue-before cursor definition-src descent "
"direction display dominant-baseline drop-initial-after-adjust drop-initial-after-align "
"drop-initial-before-adjust drop-initial-before-align drop-initial-size drop-initial-value "
"elevation empty-cells fit fit-position float float-offset font font-family font-size "
"font-size-adjust font-stretch font-style font-variant font-weight grid-columns grid-rows "
"hanging-punctuation height hyphenate-after hyphenate-before hyphenate-character hyphenate-lines "
"hyphenate-resource hyphens icon image-orientation image-resolution inline-box-align "
"insert-position left letter-spacing line-height line-stacking line-stacking-ruby "
"line-stacking-shift line-stacking-strategy list-style list-style-image list-style-position "
"list-style-type make-element margin margin-bottom margin-left margin-right margin-top mark "
"mark-after mark-before marker-offset marks marquee-direction marquee-play-count marquee-speed "
"marquee-style mathline max-height max-width min-height min-width move-to nav-down nav-index "
"nav-left nav-right nav-up opacity orphans outline outline-color outline-offset outline-style "
"outline-width overflow overflow-style overflow-x overflow-y padding padding-bottom padding-left "
"padding-right padding-top page page-break-after page-break-before page-break-inside page-policy "
"panose-1 pause pause-after pause-before perspective perspective-origin phonemes pitch "
"pitch-range play-during position presentation-level prototype prototype-insert-policy "
"prototype-insert-position punctuation-trim quotes rendering-intent resize rest rest-after "
"rest-before richness right rotation rotation-point ruby-align ruby-overhang ruby-position "
"ruby-span size slope speak speak-header speak-numeral speak-punctuation speech-rate src stemh "
"stemv stress string-set table-layout target target-name target-new target-position text-align "
"text-align-last text-decoration text-emphasis text-height text-indent text-justify text-outline "
"text-replace text-shadow text-transform text-wrap top topline transform transform-origin "
"transform-style transition transition-delay transition-duration transition-property "
"transition-timing-function unicode-bidi unicode-range units-per-em vertical-align visibility "
"voice-balance voice-duration voice-family voice-pitch voice-pitch-range voice-rate voice-stress "
"voice-volume volume white-space white-space-collapse widows width widths word-break word-spacing "
"word-wrap x-height z-index"

, // 1 Pseudo-Class
"active after before checked default disabled empty enabled first first-child first-letter "
"first-line first-of-type focus hover indeterminate invalid lang last-child last-of-type left "
"link not nth-child nth-last-child nth-last-of-type nth-of-type only-child only-of-type optional "
"required right root target valid visited"

, NULL, NULL, NULL, NULL, NULL, NULL, NULL

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_CSS[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_CSS_COMMENT, NP2STYLE_Comment, EDITSTYLE_HOLE(L"Comment"), L"fore:#608060" },
	{ SCE_CSS_TAG, 63161, EDITSTYLE_HOLE(L"HTML Tag"), L"bold; fore:#0A246A" },
	{ SCE_CSS_CLASS, 63301, EDITSTYLE_HOLE(L"Tag Class"), L"fore:#648000" },
	{ SCE_CSS_ID, 63302, EDITSTYLE_HOLE(L"Tag Id"), L"fore:#648000" },
	{ SCE_CSS_ATTRIBUTE, 63303, EDITSTYLE_HOLE(L"Tag Attribute"), L"italic; fore:#648000" },
	{ MULTI_STYLE(SCE_CSS_PSEUDOCLASS, SCE_CSS_EXTENDED_PSEUDOCLASS, SCE_CSS_PSEUDOELEMENT, SCE_CSS_EXTENDED_PSEUDOELEMENT), 63304, EDITSTYLE_HOLE(L"Pseudo Class/Element"), L"fore:#B000B0" },
	{ MULTI_STYLE(SCE_CSS_IDENTIFIER, SCE_CSS_IDENTIFIER2, SCE_CSS_IDENTIFIER3, SCE_CSS_EXTENDED_IDENTIFIER), 63306, EDITSTYLE_HOLE(L"CSS Property"), L"fore:#FF4000" },
	{ MULTI_STYLE(SCE_CSS_DOUBLESTRING, SCE_CSS_SINGLESTRING, 0, 0), NP2STYLE_String, EDITSTYLE_HOLE(L"String"), L"fore:#008000" },
	{ SCE_CSS_VALUE, 63309, EDITSTYLE_HOLE(L"Value"), L"fore:#3A6EA5" },
	{ SCE_CSS_OPERATOR, NP2STYLE_Operator, EDITSTYLE_HOLE(L"Operator"), L"fore:#B000B0" },
	{ SCE_CSS_IMPORTANT, 63310, EDITSTYLE_HOLE(L"Important"), L"bold; fore:#C80000" },
	{ SCE_CSS_DIRECTIVE, NP2STYLE_Directive, EDITSTYLE_HOLE(L"Directive"), L"bold; back:#FFF1A8" },
	{ SCE_CSS_MEDIA, 63382, EDITSTYLE_HOLE(L"Media"), L"bold; fore:#0A246A" },
	{ SCE_CSS_VARIABLE, NP2STYLE_Variable, EDITSTYLE_HOLE(L"Variable"), L"bold; fore:#FF4000" },
	{ SCE_CSS_UNKNOWN_PSEUDOCLASS, 63305, EDITSTYLE_HOLE(L"Unknown Pseudo Class"), L"fore:#C80000; back:#FFFF80" },
	{ SCE_CSS_UNKNOWN_IDENTIFIER, 63307, EDITSTYLE_HOLE(L"Unknown Property"), L"fore:#C80000; back:#FFFF80" },
};

EDITLEXER lexCSS = {
	SCLEX_CSS, NP2LEX_CSS,
	EDITLEXER_HOLE(L"CSS Style Sheet", Styles_CSS),
	L"css; scss; less; hss",
	&Keywords_CSS,
	Styles_CSS
};

