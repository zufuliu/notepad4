#include "EditLexer.h"
#include "EditStyleX.h"

// https://graphviz.org/
// http://blockdiag.com/en/
// https://graphviz.gitlab.io/_pages/doc/info/lang.html

static KEYWORDLIST Keywords_DOT = {{
"node edge graph digraph subgraph strict true false "

// blockdiag
"blockdiag group seqdiag actdiag lane nwdiag network rackdiag rack packetdiag "

, // 1 Type Keyword
// Node Shape
"box polygon ellipse circle point egg triangle plaintext diamond trapezium parallelogram house pentagon hexagon septagon octagon doublecircle doubleoctagon tripleoctagon invtriangle invtrapezium invhouse Mdiamond Msquare Mcircle rect rectangle none note tab folder box3d component "

, // 2 Preprocessor
NULL
, // 3 Directive
// HTML Like Label
"TABLE TR TD FONT IMG BR "

, // 4 Attribute
"Damping K URL arrowhead arrowsize arrowtail aspect bb bgcolor center charset clusterrank color colorList colorscheme comment compound concentrate constraint decorate defaultdist dim dimen dir diredgeconstraints distortion dpi edgeURL edgehref edgetarget edgetooltip epsilon esep fillcolor fixedsize fontcolor fontname fontnames fontpath fontsize group headURL headclip headhref headlabel headport headtarget headtooltip height href id image imagescale label labelURL labelangle labeldistance labelfloat labelfontcolor labelfontname labelfontsize labelhref labeljust labelloc labelloc labeltarget labeltooltip landscape layer layers layersep layout len levels levelsgap lhead lp ltail margin maxiter mclimit mindist minlen mode model mosek nodesep nojustify normalize nslimit nslimit1 ordering orientation orientation outputorder overlap overlap_scaling pack packmode pad page pagedir pencolor penwidth peripheries pin pos quadtree quantum rank rankdir ranksep ratio rects regular remincross repulsiveforce resolution root rotate samehead sametail samplepoints searchsize sep shape shapefile showboxes sides size skew smoothing sortv splines start style stylesheet tailURL tailclip tailhref taillabel tailport tailtarget tailtooltip target tooltip truecolor vertices viewport voro_margin weight width "
//
"ALIGN BALIGN BGCOLOR BORDER CELLBORDER CELLPADDING CELLSPACING COLOR COLSPAN FACE FIXEDSIZE HEIGHT HREF POINT-SIZE PORT ROWSPAN SCALE SRC TARGET TITLE TOOLTIP VALIGN WIDTH "

, // 5 Class
NULL
, // 6 Interface
NULL
, // 7 Enumeration
// Style for Node
"filled invisible diagonals rounded dashed dotted solid bold "

, // 8 Constant
// Color Name
"aliceblue antiquewhite aquamarine azure beige bisque black blanchedalmond blue blueviolet brown burlywood cadetblue chartreuse chocolate coral cornflowerblue cornsilk crimson cyan darkgoldenrod darkgreen darkkhaki darkolivegreen darkorange darkorchid darksalmon darkseagreen darkslateblue darkslategray darkslategrey darkturquoise darkviolet deeppink deepskyblue dimgray dimgrey dodgerblue firebrick floralwhite forestgreen gainsboro ghostwhite gold goldenrod gray green greenyellow grey honeydew hotpink indianred indigo ivory khaki lavender lavenderblush lawngreen lemonchiffon lightblue lightcoral lightcyan lightgoldenrod lightgoldenrodyellow lightgray lightgrey lightpink lightsalmon lightseagreen lightskyblue lightslateblue lightslategray lightslategrey lightsteelblue lightyellow limegreen linen magenta maroon mediumaquamarine mediumblue mediumorchid mediumpurple mediumseagreen mediumslateblue mediumspringgreen mediumturquoise mediumvioletred midnightblue mintcream mistyrose moccasin navajowhite navy navyblue oldlace olivedrab orange orangered orchid palegoldenrod palegreen paleturquoise palevioletred papayawhip peachpuff peru pink plum powderblue purple red rosybrown royalblue saddlebrown salmon sandybrown seagreen seashell sienna skyblue slateblue slategray slategrey snow springgreen steelblue tan thistle tomato transparent turquoise violet violetred wheat white whitesmoke yellow yellowgreen "

, NULL, NULL, NULL, NULL, NULL, NULL, NULL
}};

static EDITSTYLE Styles_DOT[] = {
	EDITSTYLE_DEFAULT,
	{ SCE_C_WORD, NP2StyleX_Keyword, L"fore:#0000FF" },
	{ SCE_C_WORD2, NP2StyleX_Attribute, L"fore:#FF8000" },
	{ SCE_C_LABEL, NP2StyleX_Value, L"fore:#008287" },
	{ SCE_C_DIRECTIVE, NP2StyleX_Label, L"fore:#008287" },
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, SCE_C_COMMENTDOC, 0), NP2StyleX_Comment, L"fore:#608060" },
	{ SCE_C_STRING, NP2StyleX_String, L"fore:#008000" },
	{ SCE_C_NUMBER, NP2StyleX_Number, L"fore:#FF0000" },
	{ SCE_C_OPERATOR, NP2StyleX_Operator, L"fore:#B000B0" },
};

EDITLEXER lexDOT = {
	SCLEX_GRAPHVIZ, NP2LEX_DOT,
	EDITLEXER_HOLE(L"GraphViz Dot", Styles_DOT),
	L"dot; gv; diag",
	&Keywords_DOT,
	Styles_DOT
};
