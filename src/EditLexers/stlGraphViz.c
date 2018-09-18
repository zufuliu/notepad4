#include "EditLexer.h"
#include "EditStyle.h"

// http://graphviz.org/

static KEYWORDLIST Keywords_DOT = {{
"node edge graph digraph subgraph strict true false"

, // 1 Type Keyword
// Node Shape
"box polygon ellipse circle point egg triangle plaintext diamond trapezium parallelogram house pentagon hexagon septagon octagon doublecircle doubleoctagon tripleoctagon invtriangle invtrapezium invhouse Mdiamond Msquare Mcircle rect rectangle none note tab folder box3d component "

, // 2 Preprocessor
""
, // 3 Directive
// HTML Like Label
"TABLE TR TD FONT IMG BR "

, // 4 Attribute
"Damping K URL arrowhead arrowsize arrowtail aspect bb bgcolor center charset clusterrank color colorList colorscheme comment compound concentrate constraint decorate defaultdist dim dimen dir diredgeconstraints distortion dpi edgeURL edgehref edgetarget edgetooltip epsilon esep fillcolor fixedsize fontcolor fontname fontnames fontpath fontsize group headURL headclip headhref headlabel headport headtarget headtooltip height href id image imagescale label labelURL labelangle labeldistance labelfloat labelfontcolor labelfontname labelfontsize labelhref labeljust labelloc labelloc labeltarget labeltooltip landscape layer layers layersep layout len levels levelsgap lhead lp ltail margin maxiter mclimit mindist minlen mode model mosek nodesep nojustify normalize nslimit nslimit1 ordering orientation orientation outputorder overlap overlap_scaling pack packmode pad page pagedir pencolor penwidth peripheries pin pos quadtree quantum rank rankdir ranksep ratio rects regular remincross repulsiveforce resolution root rotate samehead sametail samplepoints searchsize sep shape shapefile showboxes sides size skew smoothing sortv splines start style stylesheet tailURL tailclip tailhref taillabel tailport tailtarget tailtooltip target tooltip truecolor vertices viewport voro_margin weight width "
//
"ALIGN BALIGN BGCOLOR BORDER CELLBORDER CELLPADDING CELLSPACING COLOR COLSPAN FACE FIXEDSIZE HEIGHT HREF POINT-SIZE PORT ROWSPAN SCALE SRC TARGET TITLE TOOLTIP VALIGN WIDTH "

, // 5 Class
""
, // 6 Interface
""
, // 7 Enumeration
// Style for Node
"filled invisible diagonals rounded dashed dotted solid bold "

, // 8 Constant
// Color Name
"aliceblue antiquewhite aquamarine azure beige bisque black blanchedalmond blue blueviolet brown burlywood cadetblue chartreuse chocolate coral cornflowerblue cornsilk crimson cyan darkgoldenrod darkgreen darkkhaki darkolivegreen darkorange darkorchid darksalmon darkseagreen darkslateblue darkslategray darkslategrey darkturquoise darkviolet deeppink deepskyblue dimgray dimgrey dodgerblue firebrick floralwhite forestgreen gainsboro ghostwhite gold goldenrod gray green greenyellow grey honeydew hotpink indianred indigo ivory khaki lavender lavenderblush lawngreen lemonchiffon lightblue lightcoral lightcyan lightgoldenrod lightgoldenrodyellow lightgray lightgrey lightpink lightsalmon lightseagreen lightskyblue lightslateblue lightslategray lightslategrey lightsteelblue lightyellow limegreen linen magenta maroon mediumaquamarine mediumblue mediumorchid mediumpurple mediumseagreen mediumslateblue mediumspringgreen mediumturquoise mediumvioletred midnightblue mintcream mistyrose moccasin navajowhite navy navyblue oldlace olivedrab orange orangered orchid palegoldenrod palegreen paleturquoise palevioletred papayawhip peachpuff peru pink plum powderblue purple red rosybrown royalblue saddlebrown salmon sandybrown seagreen seashell sienna skyblue slateblue slategray slategrey snow springgreen steelblue tan thistle tomato transparent turquoise violet violetred wheat white whitesmoke yellow yellowgreen "

, "", "", "", "", "", "", ""
}};

EDITLEXER lexDOT = { SCLEX_GRAPHVIZ, NP2LEX_DOT, EDITLEXER_HOLE, L"GraphViz Dot", L"dot; gv", L"", &Keywords_DOT,
{
	{ STYLE_DEFAULT, NP2STYLE_Default, L"Default", L"", L"" },
	{ SCE_C_WORD, NP2STYLE_Keyword, L"Keyword", L"fore:#0000FF", L"" },
	{ SCE_C_WORD2, NP2STYLE_Attribute, L"Attribute", L"fore:#FF8000", L""},
	{ SCE_C_LABEL, NP2STYLE_XMLValue, L"Value", L"fore:#008287", L""},
	{ SCE_C_DIRECTIVE, NP2STYLE_Label, L"Label", L"fore:#008287", L""},
	{ MULTI_STYLE(SCE_C_COMMENT, SCE_C_COMMENTLINE, SCE_C_COMMENTDOC, 0), NP2STYLE_Comment, L"Comment", L"fore:#608060", L"" },
	{ SCE_C_STRING, NP2STYLE_String, L"String", L"fore:#008000", L"" },
	{ SCE_C_NUMBER, NP2STYLE_Number, L"Number", L"fore:#FF0000", L"" },
	{ SCE_C_OPERATOR, NP2STYLE_Operator, L"Operator", L"fore:#B000B0", L"" },
	EDITSTYLE_SENTINEL
}
};
