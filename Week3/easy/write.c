#include <syslog.h>

void write_log() {

    setlogmask(LOG_UPTO(LOG_DEBUG) | LOG_MASK(LOG_DEBUG));
    openlog("exampleprog", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

    //  syslog(LOG_NOTICE, "Program started by User %d", getuid());
    // syslog(LOG_INFO, "A tree falls in a forest");

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
int main() {

    write_log();
    return 0;
};
