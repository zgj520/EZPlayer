#pragma once

#ifndef DEFINE_BASE_QMLLIST
#define DEFINE_BASE_QMLLIST(__CLASS_NAME, CPP_LIST_NAME, NODE_NAME)                                         \
public:                                                                                                     \
    Q_PROPERTY(QQmlListProperty<NODE_NAME> NODE_NAME##List READ get##NODE_NAME##List NOTIFY NODE_NAME##ListChanged)\
    Q_INVOKABLE QQmlListProperty<NODE_NAME> get##NODE_NAME##List(){                                         \
        return { this, this,                                                                                \
                 &##__CLASS_NAME::append_##NODE_NAME,                                                       \
                 &##__CLASS_NAME::count_##NODE_NAME,                                                        \
                 &##__CLASS_NAME::at_##NODE_NAME,                                                           \
                 &##__CLASS_NAME::clear_##NODE_NAME,                                                        \
                 &##__CLASS_NAME::replace_##NODE_NAME,                                                      \
                 &##__CLASS_NAME::removeLast_##NODE_NAME};                                                  \
    }                                                                                                       \
Q_SIGNALS:                                                                                                  \
    void NODE_NAME##ListChanged();                                                                          \
public:                                                                                                     \
    void append##NODE_NAME(NODE_NAME info){                                                                 \
    }                                                                                                       \
    static void append_##NODE_NAME(QQmlListProperty<NODE_NAME>*list, NODE_NAME*p) {                         \
        reinterpret_cast<__CLASS_NAME*>(list->data)->append##NODE_NAME(*p);                                 \
    }                                                                                                       \
    qsizetype count##NODE_NAME() {                                                                          \
        return CPP_LIST_NAME.count();                                                                       \
    }                                                                                                       \
    static qsizetype count_##NODE_NAME(QQmlListProperty<NODE_NAME>* list) {                                 \
        return reinterpret_cast<__CLASS_NAME*>(list->data)->count##NODE_NAME();                             \
    }                                                                                                       \
    void clear##NODE_NAME() {                                                                               \
    }                                                                                                       \
    static void clear_##NODE_NAME(QQmlListProperty<NODE_NAME>* list) {                                      \
        reinterpret_cast<__CLASS_NAME*>(list->data)->clear##NODE_NAME();                                    \
    }                                                                                                       \
    void replace##NODE_NAME(qsizetype i, NODE_NAME*) {                                                      \
    }                                                                                                       \
    static void replace_##NODE_NAME(QQmlListProperty<NODE_NAME>* list, qsizetype i, NODE_NAME*p) {          \
        reinterpret_cast<__CLASS_NAME*>(list->data)->replace##NODE_NAME(i, p);                              \
    }                                                                                                       \
    void removeLast##NODE_NAME() {                                                                          \
    }                                                                                                       \
    static void removeLast_##NODE_NAME(QQmlListProperty<NODE_NAME>* list) {                                 \
        reinterpret_cast<__CLASS_NAME*>(list->data)->removeLast##NODE_NAME();                               \
    }
#endif // !DEFINE_QMLLIST

#ifndef DEFINE_OBJECT_QMLLIST
#define DEFINE_OBJECT_QMLLIST(__CLASS_NAME, CPP_LIST_NAME,NODE_NAME)                                        \
        DEFINE_BASE_QMLLIST(__CLASS_NAME, CPP_LIST_NAME, NODE_NAME)                                         \
        NODE_NAME* at##NODE_NAME(qsizetype i) {                                                             \
            return &##CPP_LIST_NAME[i];                                                                     \
        }                                                                                                   \
        static NODE_NAME* at_##NODE_NAME(QQmlListProperty<NODE_NAME>* list, qsizetype i) {                  \
            return reinterpret_cast<__CLASS_NAME*>(list->data)->at##NODE_NAME(i);                           \
        }
#endif

#ifndef DEFINE_PTR_QMLLIST
#define DEFINE_PTR_QMLLIST(__CLASS_NAME, CPP_LIST_NAME,NODE_NAME)                                           \
        DEFINE_BASE_QMLLIST(__CLASS_NAME, CPP_LIST_NAME, NODE_NAME)                                         \
        NODE_NAME* at##NODE_NAME(qsizetype i) {                                                             \
            return CPP_LIST_NAME[i];                                                                        \
        }                                                                                                   \
        static NODE_NAME* at_##NODE_NAME(QQmlListProperty<NODE_NAME>* list, qsizetype i) {                  \
            return reinterpret_cast<__CLASS_NAME*>(list->data)->at##NODE_NAME(i);                           \
        }
#endif
