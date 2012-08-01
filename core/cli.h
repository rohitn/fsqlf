#ifndef CLI_H
#define CLI_H



#define FAIL_WITH_ERROR( CODE , MESSAGE , ... ) \
    {                                           \
        fprintf(stderr, "\n" MESSAGE "\n", __VA_ARGS__ );\
        exit( CODE );                           \
    }


#define PRINT_OPTION_INFO( OPTION_TEXT , OPTION_DESCRIPTION ) \
    fprintf(stderr,"\t"   OPTION_TEXT   "\n\t    "   OPTION_DESCRIPTION   "\n");


#define ARGV_MATCH( INDEX , TEXT ) (strcmp(argv[ INDEX ] , TEXT ) == 0)


void usage_info(int argc, char **argv)
{
    fprintf(stderr,"usage: %s [<input_file>] [<output_file>] [options]\n", argv[0] );
    fprintf(stderr,"\t");
    fprintf(stderr,"If <output_file> or <input_file> is missing, then corresponding standard IO is used\n");
    fprintf(stderr,"options:\n");
    PRINT_OPTION_INFO( "--select-comma-newline (after|before|none)" , "New lines for each item in SELECT clause");
    PRINT_OPTION_INFO( "--select-newline-after <digit>"             , "Put <digit> new lines right after SELECT keyword");
    PRINT_OPTION_INFO( "--newline-or-before <digit>"                , "Put <digit> new lines before OR keyword");
    PRINT_OPTION_INFO( "--newline-or-after <digit>"                 , "Put <digit> new lines before OR keyword");
    PRINT_OPTION_INFO( "--newline-and-before <digit>"               , "Put <digit> new lines before AND keyword");
    PRINT_OPTION_INFO( "--newline-and-after <digit>"                , "Put <digit> new lines before AND keyword");
    PRINT_OPTION_INFO( "--newline-major-sections <digit>"           , "Put <digit> new lines before major sections (FROM, JOIN, WHERE)");
    PRINT_OPTION_INFO( "--keyword-case (upper|lower|initcap|none)"  , "Convert all keywords to UPPER, lower, or Initcap case, or not to convert case at all");
    PRINT_OPTION_INFO( "--keyword-text (original|default)"          , "Use original or programs default text for the keyword, when there are several alternatives");
    PRINT_OPTION_INFO( "--debug (none|state|match|paranthesis)"     , "Print info for debuging.  To have different kinds of debug output, use more than once");
    PRINT_OPTION_INFO( "--create-config-file"                       , "(Re)create '"CONFIG_FILE"' config file.");
    PRINT_OPTION_INFO( "--help, -h"                                 , "Show this help.");
}

void read_cli_options(int argc, char **argv)
{
    int i;
    if(argc == 1) return; // use stdin and stdout

    for(i=1;i<argc;i++)
    {
        if( argv[i][0] != '-')
        {
            if(yyin == stdin){   //try to openinig INPUT file
                if(  !(yyin = fopen(argv[1],"r"))  ) FAIL_WITH_ERROR(1,"Error opening input file: %s", argv[i]);
            }
            else if(yyout == stdout){   //try to openinig OUTPUT file (only if INPUT file is set)
                if(  !(yyout=fopen(argv[2],"w+"))  ) FAIL_WITH_ERROR(1,"Error opening output file: %s", argv[i]);
            }
        } else if( ARGV_MATCH(i,"--select-comma-newline") )
        {
            if( ++i >= argc) FAIL_WITH_ERROR(1,"Missing value for option : %s", argv[i-1]);
            if(strcmp(argv[i],"after") == 0) {
                kw_comma.nl_before = 0;
                kw_comma.nl_after  = 1;
            } else if(strcmp(argv[i],"before") == 0) {
                kw_comma.nl_before = 1;
                kw_comma.nl_after  = 0;
            } else if(strcmp(argv[i],"none") == 0) {
                kw_comma.nl_before = 0;
                kw_comma.nl_after  = 0;
            }
        } else if( ARGV_MATCH(i,"--keyword-case") )
        {
            if( ++i >= argc) FAIL_WITH_ERROR(1,"Missing value for option : %s", argv[i-1]);
            if(strcmp(argv[i],"none") == 0) {
                set_case(CASE_none);
            } else if(strcmp(argv[i],"upper") == 0) {
                set_case(CASE_UPPER);
            } else if(strcmp(argv[i],"lower") == 0) {
                set_case(CASE_lower);
            } else if(strcmp(argv[i],"initcap") == 0) {
                set_case(CASE_Initcap);
            }
        } else if( ARGV_MATCH(i,"--keyword-text") )
        {
            if( ++i >= argc) FAIL_WITH_ERROR(1,"Missing value for option : %s", argv[i-1]);
            if(strcmp(argv[i],"original") == 0) {
                set_text_original(1);
            } else if(strcmp(argv[i],"default") == 0) {
                set_text_original(0);
            }
        } else if( ARGV_MATCH(i,"--select-newline-after") )
        {
            if( ++i >= argc || !isdigit(argv[i][0]) ) FAIL_WITH_ERROR(1,"Missing or invalid value for option : %s", argv[i-1]);
            kw_select.nl_after = atoi(argv[i]);
        } else if( ARGV_MATCH(i,"--newline-or-before") )
        {
            if( ++i >= argc || !isdigit(argv[i][0]) ) FAIL_WITH_ERROR(1,"Missing or invalid value for option : %s", argv[i-1]);
            kw_or.nl_before = atoi(argv[i]);
        } else if( ARGV_MATCH(i,"--newline-or-after") )
        {
            if( ++i >= argc || !isdigit(argv[i][0]) ) FAIL_WITH_ERROR(1,"Missing or invalid value for option : %s", argv[i-1]);
            kw_or.nl_after = atoi(argv[i]);
        } else if( ARGV_MATCH(i,"--newline-and-before") )
        {
            if( ++i >= argc || !isdigit(argv[i][0]) ) FAIL_WITH_ERROR(1,"Missing or invalid value for option : %s", argv[i-1]);
            kw_and.nl_before = atoi(argv[i]);
        } else if( ARGV_MATCH(i,"--newline-and-after") )
        {
            if( ++i >= argc || !isdigit(argv[i][0]) ) FAIL_WITH_ERROR(1,"Missing or invalid value for option : %s", argv[i-1]);
            kw_and.nl_after = atoi(argv[i]);
        } else if( ARGV_MATCH(i,"--newline-major-sections") )
        {
            if( ++i >= argc || !isdigit(argv[i][0]) ) FAIL_WITH_ERROR(1,"Missing or invalid value for option : %s", argv[i-1]);
            kw_from.nl_before = atoi(argv[i]);
            kw_where.nl_before = atoi(argv[i]);
            kw_inner_join.nl_before = atoi(argv[i]);
            kw_left_join.nl_before  = atoi(argv[i]);
            kw_right_join.nl_before = atoi(argv[i]);
            kw_full_join.nl_before  = atoi(argv[i]);
            kw_cross_join.nl_before = atoi(argv[i]);
        } else if( ARGV_MATCH(i,"--debug") )
        {
            if( ++i >= argc ) FAIL_WITH_ERROR(1,"Missing or invalid value for option : %s", argv[i-1]);
            if     ( ARGV_MATCH(i,"none")         ) debug_level |= DEBUGNONE;
            else if( ARGV_MATCH(i,"state")        ) debug_level |= DEBUGSTATES;
            else if( ARGV_MATCH(i,"match")        ) debug_level |= DEBUGMATCHES;
            else if( ARGV_MATCH(i,"paranthesis")  ) debug_level |= DEBUGPARCOUNTS;
            else FAIL_WITH_ERROR(1,"Missing or invalid value for option : %s", argv[i-1]);

        } else if( strcmp(argv[i],"--create-config-file") == 0) {
			if(create_config_file() != 0)
				exit(1);
			else {
				fprintf(stderr,"File '%s' (re)created.\n", CONFIG_FILE);
				exit(0);
			}

        } else if( strcmp(argv[i],"--help") == 0 || strcmp(argv[i],"-h") == 0)
        {
            usage_info(argc, argv);
            exit(0);
        } else FAIL_WITH_ERROR(1,"Try `%s --help' for more information\n", argv[0]);
    }
}



#endif

