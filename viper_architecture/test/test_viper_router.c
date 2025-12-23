/**
 * test_viper_router.c - Unit tests for VIPER Router component
 *
 * Tests router initialization, route management, navigation, and state transitions.
 */

#include "test_framework.h"
#include "../include/viper_router.h"
#include <string.h>

/* Test router initialization */
TEST_CASE(test_router_init) {
    struct viper_router *router = viper_router_init("TestRouter", 123);
    TEST_ASSERT_NOT_NULL(router);
    TEST_ASSERT_EQUAL_STRING("TestRouter", router->router_name);
    TEST_ASSERT_EQUAL(123, router->router_id);
    TEST_ASSERT_EQUAL(ROUTER_STATE_IDLE, viper_router_get_state(router));
    viper_router_destroy(router);
}

/* Test router initialization with NULL name */
TEST_CASE(test_router_init_null_name) {
    struct viper_router *router = viper_router_init(NULL, 456);
    TEST_ASSERT_NOT_NULL(router);
    TEST_ASSERT_EQUAL_STRING("VIPER_Router", router->router_name);
    TEST_ASSERT_EQUAL(456, router->router_id);
    viper_router_destroy(router);
}

/* Test adding routes */
TEST_CASE(test_router_add_route) {
    struct viper_router *router = viper_router_init("RouteTest", 1);
    TEST_ASSERT_NOT_NULL(router);
    
    int rc = viper_router_add_route(router, "/home", "HomeScreen", 100, NULL, 0);
    TEST_ASSERT_EQUAL(0, rc);
    TEST_ASSERT_EQUAL(1, router->route_count);
    
    rc = viper_router_add_route(router, "/settings", "SettingsScreen", 101, NULL, 0);
    TEST_ASSERT_EQUAL(0, rc);
    TEST_ASSERT_EQUAL(2, router->route_count);
    
    viper_router_destroy(router);
}

/* Test adding route with context */
TEST_CASE(test_router_add_route_with_context) {
    struct viper_router *router = viper_router_init("ContextTest", 2);
    TEST_ASSERT_NOT_NULL(router);
    
    const char *context_data = "UserPrefs";
    size_t context_size = strlen(context_data) + 1;
    
    int rc = viper_router_add_route(router, "/profile", "ProfileScreen", 200, context_data, context_size);
    TEST_ASSERT_EQUAL(0, rc);
    
    /* Verify route context */
    TEST_ASSERT_NOT_NULL(router->routes[0].context);
    TEST_ASSERT_EQUAL_STRING(context_data, (const char *)router->routes[0].context);
    
    viper_router_destroy(router);
}

/* Test navigation to existing route */
TEST_CASE(test_router_navigate_success) {
    struct viper_router *router = viper_router_init("NavTest", 3);
    TEST_ASSERT_NOT_NULL(router);
    
    viper_router_add_route(router, "/dashboard", "DashboardScreen", 300, NULL, 0);
    viper_router_add_route(router, "/help", "HelpScreen", 301, NULL, 0);
    
    int rc = viper_router_navigate(router, "/dashboard", NULL, 0);
    TEST_ASSERT_EQUAL(0, rc);
    TEST_ASSERT_EQUAL(ROUTER_STATE_ROUTING, viper_router_get_state(router));
    
    const struct viper_route *current = viper_router_get_current_route(router);
    TEST_ASSERT_NOT_NULL(current);
    TEST_ASSERT_EQUAL_STRING("/dashboard", current->pattern);
    TEST_ASSERT_EQUAL_STRING("DashboardScreen", current->destination);
    TEST_ASSERT_EQUAL(300, current->destination_id);
    
    viper_router_destroy(router);
}

/* Test navigation to non-existent route */
TEST_CASE(test_router_navigate_failure) {
    struct viper_router *router = viper_router_init("NavFailTest", 4);
    TEST_ASSERT_NOT_NULL(router);
    
    viper_router_add_route(router, "/valid", "ValidScreen", 400, NULL, 0);
    
    int rc = viper_router_navigate(router, "/invalid", NULL, 0);
    TEST_ASSERT_EQUAL(-1, rc);
    TEST_ASSERT_EQUAL(ROUTER_STATE_ERROR, viper_router_get_state(router));
    
    viper_router_destroy(router);
}

/* Test navigation with data */
TEST_CASE(test_router_navigate_with_data) {
    struct viper_router *router = viper_router_init("DataNavTest", 5);
    TEST_ASSERT_NOT_NULL(router);
    
    viper_router_add_route(router, "/edit", "EditScreen", 500, NULL, 0);
    
    const char *nav_data = "EditData";
    size_t data_size = strlen(nav_data) + 1;
    
    int rc = viper_router_navigate(router, "/edit", nav_data, data_size);
    TEST_ASSERT_EQUAL(0, rc);
    
    const struct viper_navigation_context *ctx = viper_router_get_context(router);
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_NOT_NULL(ctx->transition_data);
    TEST_ASSERT_EQUAL_STRING(nav_data, (const char *)ctx->transition_data);
    
    viper_router_destroy(router);
}

/* Test navigation history (back/forward) */
TEST_CASE(test_router_navigation_history) {
    struct viper_router *router = viper_router_init("HistoryTest", 6);
    TEST_ASSERT_NOT_NULL(router);
    
    viper_router_add_route(router, "/page1", "Page1", 601, NULL, 0);
    viper_router_add_route(router, "/page2", "Page2", 602, NULL, 0);
    viper_router_add_route(router, "/page3", "Page3", 603, NULL, 0);
    
    viper_router_navigate(router, "/page1", NULL, 0);
    viper_router_navigate(router, "/page2", NULL, 0);
    viper_router_navigate(router, "/page3", NULL, 0);
    
    /* Current should be page3 */
    const struct viper_route *current = viper_router_get_current_route(router);
    TEST_ASSERT_NOT_NULL(current);
    TEST_ASSERT_EQUAL_STRING("/page3", current->pattern);
    
    /* Navigate back */
    int rc = viper_router_navigate_back(router);
    TEST_ASSERT_EQUAL(0, rc);
    current = viper_router_get_current_route(router);
    TEST_ASSERT_EQUAL_STRING("/page2", current->pattern);
    
    /* Navigate back again */
    rc = viper_router_navigate_back(router);
    TEST_ASSERT_EQUAL(0, rc);
    current = viper_router_get_current_route(router);
    TEST_ASSERT_EQUAL_STRING("/page1", current->pattern);
    
    /* Navigate forward */
    rc = viper_router_navigate_forward(router);
    TEST_ASSERT_EQUAL(0, rc);
    current = viper_router_get_current_route(router);
    TEST_ASSERT_EQUAL_STRING("/page2", current->pattern);
    
    viper_router_destroy(router);
}

/* Test navigation history boundaries */
TEST_CASE(test_router_navigation_boundaries) {
    struct viper_router *router = viper_router_init("BoundaryTest", 7);
    TEST_ASSERT_NOT_NULL(router);
    
    viper_router_add_route(router, "/only", "OnlyScreen", 700, NULL, 0);
    viper_router_navigate(router, "/only", NULL, 0);
    
    /* Cannot go back from first page */
    int rc = viper_router_navigate_back(router);
    TEST_ASSERT_EQUAL(-1, rc);
    
    /* Cannot go forward beyond last */
    rc = viper_router_navigate_forward(router);
    TEST_ASSERT_EQUAL(-1, rc);
    
    viper_router_destroy(router);
}

/* Test router state transitions */
TEST_CASE(test_router_state_transitions) {
    struct viper_router *router = viper_router_init("StateTest", 8);
    TEST_ASSERT_NOT_NULL(router);
    
    /* Initial state should be IDLE */
    TEST_ASSERT_EQUAL(ROUTER_STATE_IDLE, viper_router_get_state(router));
    
    /* Add a route and navigate */
    viper_router_add_route(router, "/test", "TestScreen", 800, NULL, 0);
    viper_router_navigate(router, "/test", NULL, 0);
    
    /* Should be in ROUTING state after successful navigation */
    TEST_ASSERT_EQUAL(ROUTER_STATE_ROUTING, viper_router_get_state(router));
    
    /* Navigate to invalid route to trigger ERROR state */
    viper_router_navigate(router, "/invalid", NULL, 0);
    TEST_ASSERT_EQUAL(ROUTER_STATE_ERROR, viper_router_get_state(router));
    
    viper_router_destroy(router);
}

/* Test router context retrieval */
TEST_CASE(test_router_context) {
    struct viper_router *router = viper_router_init("ContextTest", 9);
    TEST_ASSERT_NOT_NULL(router);
    
    viper_router_add_route(router, "/ctx", "CtxScreen", 900, NULL, 0);
    viper_router_navigate(router, "/ctx", NULL, 0);
    
    const struct viper_navigation_context *ctx = viper_router_get_context(router);
    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_EQUAL(1, ctx->history_size);
    TEST_ASSERT_EQUAL(0, ctx->history_index);
    
    viper_router_destroy(router);
}

/* Test suite runner */
int main(void) {
    printf("=== VIPER Router Unit Tests ===\n");
    
    void (*test_cases[])(void) = {
        test_router_init,
        test_router_init_null_name,
        test_router_add_route,
        test_router_add_route_with_context,
        test_router_navigate_success,
        test_router_navigate_failure,
        test_router_navigate_with_data,
        test_router_navigation_history,
        test_router_navigation_boundaries,
        test_router_state_transitions,
        test_router_context
    };
    
    RUN_TEST_SUITE(VIPER_Router, test_cases);
    print_test_summary();
    
    return test_fail_count > 0 ? 1 : 0;
}
