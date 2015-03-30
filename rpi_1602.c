#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <gio/gio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rpi_1602_gpio.h"

/* Static variables */
static GMainLoop *loop;
static GDBusConnection *connection;
static int pipefd[2];
static const gchar introspection_xml[] =
	"<node>"
	"  <interface name='rpi1602.DBus'>"
	"    <method name='SetText'>"
	"      <arg type='s' name='message' direction='in'/>"
	"    </method>"
	"  </interface>"
	"</node>";

/* Static function declarations */
static void handle_method_call(GDBusConnection *conn,
                               const gchar *sender,
                               const gchar *object_path,
                               const gchar *interface_name,
                               const gchar *method_name,
                               GVariant *parameters,
                               GDBusMethodInvocation *invocation,
                               gpointer user_data);
static void cleanup();
static void daemonize(char *name);
static void sig_handler(int signo);

static const GDBusInterfaceVTable interface_vtable = {
	&handle_method_call
};
/* Main program */
int main(int argc, char *argv[])
{
	if (pipe(pipefd) < 0) {
		exit(EXIT_FAILURE);
	}
 
	daemonize(argv[0]);
 
 	rpi_1602_gpio_init();
 	rpi_1602_gpio_update("Initializing...");
	// set up the SIGINT signal handler
	if (signal(SIGINT, &sig_handler) == SIG_ERR) {
		log_err("Failed to register SIGINT handler, quitting...\n");
		exit(EXIT_FAILURE);
	}
 
	loop = g_main_loop_new(NULL, FALSE);
 
	// get the bus connection and print out its unique name
	GError *error = NULL;
	connection = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
	if (error != NULL) {
		log_err("Failed to acquire connection, quitting...\n");
		close(pipefd[1]);
		exit(EXIT_FAILURE);
	} else {
		gchar *conn_name;
		g_object_get(connection, "unique-name", &conn_name, NULL);
		write(pipefd[1], conn_name, strlen(conn_name) + 1);
		close(pipefd[1]);
		g_free(conn_name);
	}
 
	// register our object
	GDBusNodeInfo *introspection_data = g_dbus_node_info_new_for_xml(introspection_xml, NULL);
	GDBusInterfaceInfo *interface_info = g_dbus_node_info_lookup_interface(introspection_data, "dradtke.DBusDaemon");
	g_dbus_connection_register_object(connection,
	                                  "/dradtke/DBusDaemon",
	                                  interface_info,
	                                  &interface_vtable,
	                                  NULL,
	                                  NULL,
	                                  NULL);
 
	// start the loop
 	rpi_1602_gpio_update("Done");
	log_dbg("Entering main loop");
	g_main_loop_run(loop);
 
	log_dbg("Exiting...");
	g_main_loop_unref(loop);
	return 0;
}
 
/*************** Static functions ****************/

static void cleanup()
{
	closelog();
}
 
static void daemonize(char *name)
{
	pid_t pid, sid;
	
	pid = fork();
	if (pid != 0) {
		if (pid == -1) {
			close(pipefd[0]);
			close(pipefd[1]);
			exit(EXIT_FAILURE);
		} else {
			close(pipefd[1]);
			printf("%d\n", pid);
			char buf[32];
			while (read(pipefd[0], buf, sizeof(buf)) > 0) {
				printf("%s", buf);
			}
			printf("\n");
			close(pipefd[0]);
			exit(EXIT_SUCCESS);
		}
	}
 
	close(pipefd[0]);
	umask(0);
	openlog(name, LOG_NOWAIT|LOG_PID, LOG_USER);
	atexit(&cleanup);
	log_dbg("Starting daemon...\n");
 
	if ((sid = setsid()) < 0) {
		log_err("Could not create process group\n");
		exit(EXIT_FAILURE);
	}
 
	if ((chdir("/")) < 0) {
		log_err("Could not switch to root directory\n");
		exit(EXIT_FAILURE);
	}
 
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}
 
static void handle_method_call(GDBusConnection *conn,
                               const gchar *sender,
                               const gchar *object_path,
                               const gchar *interface_name,
                               const gchar *method_name,
                               GVariant *parameters,
                               GDBusMethodInvocation *invocation,
                               gpointer user_data)
{
	if (!g_strcmp0(method_name, "SetText")) {
		gchar *message;
		g_variant_get(parameters, "(s)", &message);
		rpi_1602_gpio_update(message);
		g_free(message);
	}
}

static void sig_handler(int signo)
{
	if (signo == SIGINT) {
		g_main_loop_quit(loop);
	}
}
