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
static char uschar = 10;
static int e_count, g_count;

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

static void print_attr_detail(struct element *e, struct attr *a, void *arg)
{
	int *a_count = arg;
	char *rx_u, *tx_u;
	int rxprec, txprec;

	double rx = unit_value2str(rate_get_total(&a->a_rx_rate),
				   a->a_def->ad_unit,
				   &rx_u, &rxprec);
	double tx = unit_value2str(rate_get_total(&a->a_tx_rate),
				   a->a_def->ad_unit,
				   &tx_u, &txprec);

	if (*a_count > 0)
		printf(",");
	printf("\n        ");
	json_print_string(a->a_def->ad_name);
	printf(": { \"desc\": ");
	json_print_string(a->a_def->ad_description);
	printf(", \"rx\": [%.*f,", rxprec, rx);
	json_print_string(rx_u);
	printf("], \"tx\": [%.*f,", txprec, tx);
	json_print_string(tx_u);
	printf("]}");
	(*a_count)++;
}

static void json_draw_element(struct element_group *g, struct element *e,
			      void *arg)
{
	int a_count = 0;

	if (e_count > 0)
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
	e_count++;
}

static void json_draw_group(struct element_group *g, void *arg)
{
	if (g_count > 0)
		printf(",\n");
	printf("  { \"name\": ");
	json_print_string(g->g_name);
	printf(", \"elements\": [");
	e_count = 0;
	group_foreach_element(g, json_draw_element, NULL);
	if (e_count > 0)
		printf("\n  ");
	printf("]}");
	g_count++;
}

static void json_draw(void)
{
	printf("[\n");
	g_count = 0;
	group_foreach(json_draw_group, NULL);
	if (g_count > 0)
		printf("\n");
	printf("]\n%c", uschar);
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
	"  Options:\n" \
	"    quitafter=NUM  Quit bmon after NUM outputs\n" \
	"    uschar=CHAR    Unit separator character (default: \\x0a)\n");
}

static void json_parse_opt(const char *type, const char *value)
{
	if (!strcasecmp(type, "uschar") && value)
		uschar = value[0];
	else if (!strcasecmp(type, "quitafter") && value)
		c_quit_after = strtol(value, NULL, 0);
	else if (!strcasecmp(type, "help")) {
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
