#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

void write_log(int LOG_LEVEL) {

    setlogmask(LOG_UPTO(LOG_LEVEL));
    openlog("log_given_level", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

    syslog(LOG_EMERG, "System is unusable");
    syslog(LOG_ALERT, "Action must be taken immediately");
    syslog(LOG_CRIT, "Critical conditions");
    syslog(LOG_ERR, "Error conditions");
    syslog(LOG_WARNING, "Warning conditions");
    syslog(LOG_NOTICE, "Normal but significant condition");
    syslog(LOG_INFO, "Informational");
    syslog(LOG_DEBUG, "Debug-level messages");
    closelog();
}
static int verbose_flag;
static struct option long_options[] =
{
    /* These options set a flag. */
    {"verbose", no_argument,       &verbose_flag, 1},
    {"brief",   no_argument,       &verbose_flag, 0},
    /* These options don't set a flag.*/
    {"debug-level",  required_argument, 0, 'd'},
    {0, 0, 0, 0}
};

/* Flag set by ‘--verbose’. */
int main (int argc,char*  argv[])
{
    int c, log_bit_mask;
    int option_index = 0;
    while ((c = getopt_long (argc, argv, "d:", long_options, &option_index)) != -1 )
    {
        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;

            case 'd':
                log_bit_mask = atoi(optarg);
                break;

            case '?':
                /* getopt_long already printed an error message. */
                break;

            default:
                abort();
        }
    }

    /* Instead of reporting ‘--verbose’
     *      *       and ‘--brief’ as they are encountered,
     *           *             we report the final status resulting from them. */
    if (verbose_flag)
        puts ("verbose flag is set");

    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
    }

    //Execute logs
    write_log(log_bit_mask);
    exit (0);
}
