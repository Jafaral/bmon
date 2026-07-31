/*
 * out_json.c		JSON Output
 *
 * Copyright (c) 2017 Paul Miller <paul@jettero.pl>
 * Copyright (c) 2001-2013 Thomas Graf <tgraf@suug.ch>
 * Copyright (c) 2013 Red Hat, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <bmon/bmon.h>
#include <bmon/conf.h>
#include <bmon/output.h>
#include <bmon/group.h>
#include <bmon/element.h>
#include <bmon/attr.h>
#include <bmon/utils.h>

static int c_quit_after = -1;
static char c_rschar = '\n';

static void json_print_string(const char *s)
{
	putchar('"');

	if (s) {
		const unsigned char *p;

		for (p = (const unsigned char *) s; *p; p++) {
			switch (*p) {
			case '"':
				fputs("\\\"", stdout);
				break;
			case '\\':
				fputs("\\\\", stdout);
				break;
			case '\b':
				fputs("\\b", stdout);
				break;
			case '\f':
				fputs("\\f", stdout);
				break;
			case '\n':
				fputs("\\n", stdout);
				break;
			case '\r':
				fputs("\\r", stdout);
				break;
			case '\t':
				fputs("\\t", stdout);
				break;
			default:
				if (*p < 0x20)
					printf("\\u%04x", *p);
				else
					putchar(*p);
				break;
			}
		}
	}

	putchar('"');
}

static void json_print_float(double v)
{
	long long iv;

	if (!isfinite(v)) {
		fputs("null", stdout);
		return;
	}

	/* Locale-independent: JSON requires '.' as the decimal separator */
	if (v < 0) {
		putchar('-');
		v = -v;
	}
	iv = (long long) (v * 100.0 + 0.5);
	printf("%lld.%02d", iv / 100, (int) (iv % 100));
}

static void print_attr_detail(struct element *e BMON_UNUSED, struct attr *a,
			      void *arg)
{
	int *a_count = arg;
	const char *unit = a->a_def->ad_unit ? a->a_def->ad_unit->u_name : "";

	if (*a_count > 0)
		printf(",");
	printf("\n        ");
	json_print_string(a->a_def->ad_name);
	printf(": { \"desc\": ");
	json_print_string(a->a_def->ad_description);
	printf(", \"unit\": ");
	json_print_string(unit);
	printf(", \"rx\": %" PRIu64 ", \"tx\": %" PRIu64
	       ", \"rx_rate\": ",
	       rate_get_total(&a->a_rx_rate),
	       rate_get_total(&a->a_tx_rate));
	json_print_float(a->a_rx_rate.r_rate);
	printf(", \"tx_rate\": ");
	json_print_float(a->a_tx_rate.r_rate);
	printf("}");
	(*a_count)++;
}

static void json_draw_element(struct element_group *g BMON_UNUSED,
			      struct element *e, void *arg)
{
	int *e_count = arg;
	int a_count = 0;

	if (*e_count > 0)
		printf(",");
	printf("\n    {\n      \"name\": ");
	json_print_string(e->e_name);
	if (e->e_id)
		printf(",\n      \"id\": %u", e->e_id);
	if (e->e_parent) {
		printf(",\n      \"parent\": ");
		json_print_string(e->e_parent->e_name);
	}
	printf(",\n      \"attrs\": {");
	element_foreach_attr(e, print_attr_detail, &a_count);
	if (a_count > 0)
		printf("\n      ");
	printf("}\n    }");
	(*e_count)++;
}

static void json_draw_group(struct element_group *g, void *arg)
{
	int *g_count = arg;
	int e_count = 0;

	if (*g_count > 0)
		printf(",\n");
	printf("  { \"name\": ");
	json_print_string(g->g_name);
	printf(", \"elements\": [");
	group_foreach_element(g, json_draw_element, &e_count);
	if (e_count > 0)
		printf("\n  ");
	printf("]}");
	(*g_count)++;
}

static void json_draw(void)
{
	int g_count = 0;

	printf("{\n  \"timestamp\": %lld,\n  \"groups\": [\n",
	       (long long) time(NULL));
	group_foreach(json_draw_group, &g_count);
	if (g_count > 0)
		printf("\n");
	printf("  ]\n}%c", c_rschar);
	fflush(stdout);

	if (c_quit_after > 0)
		if (--c_quit_after == 0)
			exit(0);
}

static void print_help(void)
{
	printf(
	"json - JSON output\n" \
	"\n" \
	"  Prints one JSON object per update for scripting / streaming.\n" \
	"  Each object has a Unix timestamp and a groups array, followed\n" \
	"  by the record separator (default: newline).\n" \
	"\n" \
	"  Example:\n" \
	"      bmon -p eth0 -o 'json:quitafter=1'\n" \
	"\n" \
	"  Options:\n" \
	"    quitafter=NUM  Quit bmon after NUM outputs\n" \
	"    rschar=CHAR    Record separator after each JSON object\n" \
	"                   (default: \\n)\n");
}

static void json_parse_opt(const char *type, const char *value)
{
	if (!strcasecmp(type, "rschar")) {
		if (value && value[0] != '\0')
			c_rschar = value[0];
	} else if (!strcasecmp(type, "quitafter") && value) {
		char *endptr;

		c_quit_after = strtol(value, &endptr, 0);
		if (*endptr != '\0')
			quit("Invalid value for quitafter: '%s'\n", value);
	} else if (!strcasecmp(type, "help")) {
		print_help();
		exit(0);
	} else
		quit("Unknown module option '%s'\n", type);
}

static struct bmon_module json_ops = {
	.m_name		= "json",
	.m_do		= json_draw,
	.m_parse_opt	= json_parse_opt,
};

static void __init json_init(void)
{
	output_register(&json_ops);
}
