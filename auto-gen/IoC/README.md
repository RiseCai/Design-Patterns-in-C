IoC and DI
=================

* DI - Dependency injection
* IOC - Inversion of control

```
                 +––––––––––––––––––––––––––+
              +––+ Constructor way          |
+–––––––+     |  +––––––––––––––––––––––––––+
|  IoC  |     |
+––+––––+     |  +––––––––––––––––––––––––––+
   |          +––+Exposing setter and getter|
+––+––––+     |  +––––––––––––––––––––––––––+
|  DI   +–––––+
+–––––––+     |  +––––––––––––––––––––––––––+
              +––+ Interface implementation |
              |  +––––––––––––––––––––––––––+
              |
              |  +––––––––––––––––––––––––––+
              +––+ Service locator          |
                 +––––––––––––––––––––––––––+
```

This folder demonstrates the styles above in C:

| Style | Code entry |
|-------|------------|
| Constructor injection | `app_service_init(svc, repository)` |
| Setter injection | `app_service_set_notifier(svc, notifier)` |
| Method injection | `app_service_create_user(svc, name, formatter)` |
| Interface injection | `injectable_inject_notifier(&svc.injectable, notifier)` |
| Service locator | `service_locator_register` / `service_locator_resolve` |

`app_service` never `malloc`s its collaborators; the test (composition root) wires them.
