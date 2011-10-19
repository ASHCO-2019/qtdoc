/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "node.h"
#include "tree.h"
#include "codemarker.h"
#include <QUuid>
#include <qdebug.h>

QT_BEGIN_NAMESPACE

ExampleNodeMap ExampleNode::exampleNodeMap;

/*!
  \class Node
  \brief The Node class is a node in the Tree.

  A Node represents a class or function or something else
  from the source code..
 */

/*!
  When this Node is destroyed, if it has a parent Node, it
  removes itself from the parent node's child list.
 */
Node::~Node()
{
    if (par)
        par->removeChild(this);
    if (rel)
        rel->removeRelated(this);
}

/*!
  Sets this Node's Doc to \a doc. If \a replace is false and
  this Node already has a Doc, a warning is reported that the
  Doc is being overridden, and it reports where the previous
  Doc was found. If \a replace is true, the Doc is replaced
  silently.
 */
void Node::setDoc(const Doc& doc, bool replace)
{
    if (!d.isEmpty() && !replace) {
        doc.location().warning(tr("Overrides a previous doc"));
        d.location().warning(tr("(The previous doc is here)"));
    }
    d = doc;
}

/*!
  Construct a node with the given \a type and having the
  given \a parent and \a name. The new node is added to the
  parent's child list.
 */
Node::Node(Type type, InnerNode *parent, const QString& name)
    : typ(type),
      acc(Public),
      saf(UnspecifiedSafeness),
      pageTyp(NoPageType),
      sta(Commendable),
      par(parent),
      rel(0),
      nam(name)
{
    if (par)
        par->addChild(this);
}

/*!
  Returns the node's URL.
 */
QString Node::url() const
{
    return u;
}

/*!
  Sets the node's URL to \a url
 */
void Node::setUrl(const QString &url)
{
    u = url;
}

/*!
  Returns the page type as a string for use as an
  attribute value in XML or HTML.
 */
QString Node::pageTypeString() const
{
    switch (pageTyp) {
        case Node::ApiPage:
            return "api";
        case Node::ArticlePage:
            return "article";
        case Node::ExamplePage:
            return "example";
        case Node::HowToPage:
            return "howto";
        case Node::OverviewPage:
            return "overview";
        case Node::TutorialPage:
            return "tutorial";
        case Node::FAQPage:
            return "faq";
        default:
            return "article";
    }
}

/*!
  Set the page type according to the string \a t.
 */
void Node::setPageType(const QString& t)
{
    if ((t == "API") || (t == "api"))
        pageTyp = ApiPage;
    else if (t == "howto")
        pageTyp = HowToPage;
    else if (t == "overview")
        pageTyp = OverviewPage;
    else if (t == "tutorial")
        pageTyp = TutorialPage;
    else if (t == "howto")
        pageTyp = HowToPage;
    else if (t == "article")
        pageTyp = ArticlePage;
    else if (t == "example")
        pageTyp = ExamplePage;
}

/*!
  Sets the pointer to the node that this node relates to.
 */
void Node::setRelates(InnerNode *pseudoParent)
{
    if (rel) {
        rel->removeRelated(this);
    }
    rel = pseudoParent;
    pseudoParent->related.append(this);
}

/*!
  This function creates a pair that describes a link.
  The pair is composed from \a link and \a desc. The
  \a linkType is the map index the pair is filed under.
 */
void Node::setLink(LinkType linkType, const QString &link, const QString &desc)
{
    QPair<QString,QString> linkPair;
    linkPair.first = link;
    linkPair.second = desc;
    linkMap[linkType] = linkPair;
}

/*!
    Sets the information about the project and version a node was introduced
    in. The string is simplified, removing excess whitespace before being
    stored.
*/
void Node::setSince(const QString &since)
{
    sinc = since.simplified();
}

/*!
  Returns a string representing the access specifier.
 */
QString Node::accessString() const
{
    switch (acc) {
    case Protected:
        return "protected";
    case Private:
        return "private";
    case Public:
    default:
        break;
    }
    return "public";
}

/*!
  Extract a class name from the type \a string and return it.
 */
QString Node::extractClassName(const QString &string) const
{
    QString result;
    for (int i=0; i<=string.size(); ++i) {
        QChar ch;
        if (i != string.size())
            ch = string.at(i);

        QChar lower = ch.toLower();
        if ((lower >= QLatin1Char('a') && lower <= QLatin1Char('z')) ||
            ch.digitValue() >= 0 ||
            ch == QLatin1Char('_') ||
            ch == QLatin1Char(':')) {
            result += ch;
        }
        else if (!result.isEmpty()) {
            if (result != QLatin1String("const"))
                return result;
            result.clear();
        }
    }
    return result;
}

/*!
  Returns a string representing the access specifier.
 */
QString RelatedClass::accessString() const
{
    switch (access) {
    case Node::Protected:
        return "protected";
    case Node::Private:
        return "private";
    case Node::Public:
    default:
        break;
    }
    return "public";
}

/*!
 */
Node::Status Node::inheritedStatus() const
{
    Status parentStatus = Commendable;
    if (par)
        parentStatus = par->inheritedStatus();
    return (Status)qMin((int)sta, (int)parentStatus);
}

/*!
  Returns the thread safeness value for whatever this node
  represents. But if this node has a parent and the thread
  safeness value of the parent is the same as the thread
  safeness value of this node, what is returned is the
  value \c{UnspecifiedSafeness}. Why?
 */
Node::ThreadSafeness Node::threadSafeness() const
{
    if (par && saf == par->inheritedThreadSafeness())
        return UnspecifiedSafeness;
    return saf;
}

/*!
  If this node has a parent, the parent's thread safeness
  value is returned. Otherwise, this node's thread safeness
  value is returned. Why?
 */
Node::ThreadSafeness Node::inheritedThreadSafeness() const
{
    if (par && saf == UnspecifiedSafeness)
        return par->inheritedThreadSafeness();
    return saf;
}

/*!
  Returns the sanitized file name without the path.
  If the the file is an html file, the html suffix
  is removed. Why?
 */
QString Node::fileBase() const
{
    QString base = name();
    if (base.endsWith(".html"))
        base.chop(5);
    base.replace(QRegExp("[^A-Za-z0-9]+"), " ");
    base = base.trimmed();
    base.replace(" ", "-");
    return base.toLower();
}

/*!
  Returns this node's Universally Unique IDentifier as a
  QString. Creates the UUID first, if it has not been created.
 */
QString Node::guid() const
{
    if (uuid.isEmpty()) {
        QUuid quuid = QUuid::createUuid();
        QString t = quuid.toString();
        uuid = "id-" + t.mid(1,t.length()-2);
    }
    return uuid;
}

/*!
  Composes a string to be used as an href attribute in DITA
  XML. It is composed of the file name and the UUID separated
  by a '#'. If this node is a class node, the file name is
  taken from this node; if this node is a function node, the
  file name is taken from the parent node of this node.
 */
QString Node::ditaXmlHref()
{
    QString href;
    if ((type() == Function) ||
        (type() == Property) ||
        (type() == Variable)) {
        href = parent()->fileBase();
    }
    else {
        href = fileBase();
    }
    if (!href.endsWith(".xml"))
        href += ".xml";
    return href + "#" + guid();
}

/*!
  If this node is a QML class node, return a pointer to it.
  If it is a child of a QML class node, return a pointer to
  the QML class node. Otherwise, return 0;
 */
const QmlClassNode* Node::qmlClassNode() const
{
    if (isQmlNode()) {
        const Node* n = this;
        while (n && n->subType() != Node::QmlClass)
            n = n->parent();
        if (n && n->subType() == Node::QmlClass)
            return static_cast<const QmlClassNode*>(n);
    }
    return 0;
}

/*!
  If this node is a QML node, find its QML class node,
  and return a pointer to the C++ class node from the
  QML class node. That pointer will be null if the QML
  class node is a component. It will be non-null if
  the QML class node is a QML element.
 */
const ClassNode* Node::declarativeCppNode() const
{
    const QmlClassNode* qcn = qmlClassNode();
    if (qcn)
        return qcn->classNode();
    return 0;
}

/*!
  \class InnerNode
 */

/*!
  The inner node destructor deletes the children and removes
  this node from its related nodes.
 */
InnerNode::~InnerNode()
{
    deleteChildren();
    removeFromRelated();
}

/*!
  Find the node in this node's children that has the
  given \a name. If this node is a QML class node, be
  sure to also look in the children of its property
  group nodes. Return the matching node or 0.
 */
Node *InnerNode::findNode(const QString& name)
{
    Node *node = childMap.value(name);
    if (node && node->subType() != QmlPropertyGroup)
        return node;
    if ((type() == Fake) && (subType() == QmlClass)) {
        for (int i=0; i<children.size(); ++i) {
            Node* n = children.at(i);
            if (n->subType() == QmlPropertyGroup) {
                node = static_cast<InnerNode*>(n)->findNode(name);
                if (node)
                    return node;
            }
        }
    }
    return primaryFunctionMap.value(name);
}
void InnerNode::findNodes(const QString& name, QList<Node*>& n)
{
    n.clear();
    Node* node = 0;
    QList<Node*> nodes = childMap.values(name);
    /*
      <sigh> If this node's child map contains no nodes named
      name, then if this node is a QML class, seach each of its
      property group nodes for a node named name. If a match is
      found, append it to the output list and return immediately.
     */
    if (nodes.isEmpty()) {
        if ((type() == Fake) && (subType() == QmlClass)) {
            for (int i=0; i<children.size(); ++i) {
                node = children.at(i);
                if (node->subType() == QmlPropertyGroup) {
                    node = static_cast<InnerNode*>(node)->findNode(name);
                    if (node) {
                        n.append(node);
                        return;
                    }
                }
            }
        }
    }
    else {
        /*
          If the childMap does contain one or more nodes named
          name, traverse the list of matching nodes. Append each
          matching node that is not a property group node to the
          output list. Search each property group node for a node
          named name and append that node to the output list.
          This is overkill, I think, but should produce a useful
          list.
         */
        for (int i=0; i<nodes.size(); ++i) {
            node = nodes.at(i);
            if (node->subType() != QmlPropertyGroup)
                n.append(node);
            else {
                node = static_cast<InnerNode*>(node)->findNode(name);
                if (node)
                    n.append(node);
            }
        }
    }
    if (!n.isEmpty())
        return;
    node = primaryFunctionMap.value(name);
    if (node)
        n.append(node);
}

/*!
  Find the node in this node's children that has the given \a name. If
  this node is a QML class node, be sure to also look in the children
  of its property group nodes. Return the matching node or 0.

  If \a qml is true, only match a node for which node->isQmlNode()
  returns true. If \a qml is false, only match a node for which
  node->isQmlNode() returns false.
 */
Node* InnerNode::findNode(const QString& name, bool qml)
{
    QList<Node*> nodes = childMap.values(name);
    if (!nodes.isEmpty()) {
        for (int i=0; i<nodes.size(); ++i) {
            Node* node = nodes.at(i);
            if (!qml) {
                if (!node->isQmlNode())
                    return node;
            }
            else if (node->isQmlNode() && (node->subType() != QmlPropertyGroup))
                return node;
        }
    }
    if (qml && (type() == Fake) && (subType() == QmlClass)) {
        for (int i=0; i<children.size(); ++i) {
            Node* node = children.at(i);
            if (node->subType() == QmlPropertyGroup) {
                node = static_cast<InnerNode*>(node)->findNode(name);
                if (node)
                    return node;
            }
        }
    }
    return primaryFunctionMap.value(name);
}

/*!
  Same as the other findNode(), but if the node with the
  specified \a name is not of the specified \a type, return
  0.
 */
Node *InnerNode::findNode(const QString& name, Type type)
{
    if (type == Function) {
        return primaryFunctionMap.value(name);
    }
    else {
        Node *node = childMap.value(name);
        if (node && node->type() == type) {
            return node;
        }
        else {
            return 0;
        }
    }
}

/*!
  Find the function node in this node for the function named \a name.
 */
FunctionNode *InnerNode::findFunctionNode(const QString& name)
{
    return static_cast<FunctionNode *>(primaryFunctionMap.value(name));
}

/*!
  Find the function node in this node that has the same name as \a clone.
 */
FunctionNode *InnerNode::findFunctionNode(const FunctionNode *clone)
{
    QMap<QString, Node *>::ConstIterator c =
            primaryFunctionMap.find(clone->name());
    if (c != primaryFunctionMap.end()) {
        if (isSameSignature(clone, (FunctionNode *) *c)) {
            return (FunctionNode *) *c;
        }
        else if (secondaryFunctionMap.contains(clone->name())) {
            const NodeList& secs = secondaryFunctionMap[clone->name()];
            NodeList::ConstIterator s = secs.begin();
            while (s != secs.end()) {
                if (isSameSignature(clone, (FunctionNode *) *s))
                    return (FunctionNode *) *s;
                ++s;
            }
        }
    }
    return 0;
}

/*!
  Returns the list of keys from the primary function map.
 */
QStringList InnerNode::primaryKeys()
{
    QStringList t;
    QMap<QString, Node*>::iterator i = primaryFunctionMap.begin();
    while (i != primaryFunctionMap.end()) {
        t.append(i.key());
        ++i;
    }
    return t;
}

/*!
  Returns the list of keys from the secondary function map.
 */
QStringList InnerNode::secondaryKeys()
{
    QStringList t;
    QMap<QString, NodeList>::iterator i = secondaryFunctionMap.begin();
    while (i != secondaryFunctionMap.end()) {
        t.append(i.key());
        ++i;
    }
    return t;
}

/*!
 */
void InnerNode::setOverload(const FunctionNode *func, bool overlode)
{
    Node *node = (Node *) func;
    Node *&primary = primaryFunctionMap[func->name()];

    if (secondaryFunctionMap.contains(func->name())) {
        NodeList& secs = secondaryFunctionMap[func->name()];
        if (overlode) {
            if (primary == node) {
                primary = secs.first();
                secs.erase(secs.begin());
                secs.append(node);
            }
            else {
                secs.removeAll(node);
                secs.append(node);
            }
        }
        else {
            if (primary != node) {
                secs.removeAll(node);
                secs.prepend(primary);
                primary = node;
            }
        }
    }
}

/*!
  Mark all child nodes that have no documentation as having
  private access and internal status. qdoc will then ignore
  them for documentation purposes.

  \note Exception: Name collision nodes are not marked
  private/internal.
 */
void InnerNode::makeUndocumentedChildrenInternal()
{
    foreach (Node *child, childNodes()) {
        if (child->doc().isEmpty()) {
            if (child->subType() != Node::Collision) {
                child->setAccess(Node::Private);
                child->setStatus(Node::Internal);
            }
        }
    }
}

/*!
  In each child node that is a collision node,
  clear the current child pointer.
 */
void InnerNode::clearCurrentChildPointers()
{
    foreach (Node* child, childNodes()) {
        if (child->subType() == Collision) {
            child->clearCurrentChild();
        }
    }
}

/*!
 */
void InnerNode::normalizeOverloads()
{
    QMap<QString, Node *>::Iterator p1 = primaryFunctionMap.begin();
    while (p1 != primaryFunctionMap.end()) {
        FunctionNode *primaryFunc = (FunctionNode *) *p1;
        if (secondaryFunctionMap.contains(primaryFunc->name()) &&
            (primaryFunc->status() != Commendable ||
             primaryFunc->access() == Private)) {

            NodeList& secs = secondaryFunctionMap[primaryFunc->name()];
            NodeList::ConstIterator s = secs.begin();
            while (s != secs.end()) {
                FunctionNode *secondaryFunc = (FunctionNode *) *s;

                // Any non-obsolete, non-compatibility, non-private functions
                // (i.e, visible functions) are preferable to the primary
                // function.

                if (secondaryFunc->status() == Commendable &&
                    secondaryFunc->access() != Private) {

                    *p1 = secondaryFunc;
                    int index = secondaryFunctionMap[primaryFunc->name()].indexOf(secondaryFunc);
                    secondaryFunctionMap[primaryFunc->name()].replace(index, primaryFunc);
                    break;
                }
                ++s;
            }
        }
        ++p1;
    }

    QMap<QString, Node *>::ConstIterator p = primaryFunctionMap.begin();
    while (p != primaryFunctionMap.end()) {
        FunctionNode *primaryFunc = (FunctionNode *) *p;
        if (primaryFunc->isOverload())
            primaryFunc->ove = false;
        if (secondaryFunctionMap.contains(primaryFunc->name())) {
            NodeList& secs = secondaryFunctionMap[primaryFunc->name()];
            NodeList::ConstIterator s = secs.begin();
            while (s != secs.end()) {
                FunctionNode *secondaryFunc = (FunctionNode *) *s;
                if (!secondaryFunc->isOverload())
                    secondaryFunc->ove = true;
                ++s;
            }
        }
        ++p;
    }

    NodeList::ConstIterator c = childNodes().begin();
    while (c != childNodes().end()) {
        if ((*c)->isInnerNode())
            ((InnerNode *) *c)->normalizeOverloads();
        ++c;
    }
}

/*!
 */
void InnerNode::removeFromRelated()
{
    while (!related.isEmpty()) {
        Node *p = static_cast<Node *>(related.takeFirst());

        if (p != 0 && p->relates() == this) p->clearRelated();
    }
}

/*!
  Deletes all this node's children.
 */
void InnerNode::deleteChildren()
{
    NodeList childrenCopy = children; // `children` will be changed in ~Node()
    qDeleteAll(childrenCopy);
}

/*! \fn bool InnerNode::isInnerNode() const
  Returns true because this is an inner node.
 */

/*!
 */
const Node *InnerNode::findNode(const QString& name) const
{
    InnerNode *that = (InnerNode *) this;
    return that->findNode(name);
}

/*!
  If \a qml is true, only match a node for which node->isQmlNode()
  returns true. If \a qml is false, only match a node for which
  node->isQmlNode() returns false.
 */
const Node* InnerNode::findNode(const QString& name, bool qml) const
{
    InnerNode*that = (InnerNode*) this;
    return that->findNode(name, qml);
}

/*!
 */
const Node *InnerNode::findNode(const QString& name, Type type) const
{
    InnerNode *that = (InnerNode *) this;
    return that->findNode(name, type);
}

/*!
  Find the function node in this node that has the given \a name.
 */
const FunctionNode *InnerNode::findFunctionNode(const QString& name) const
{
    InnerNode *that = (InnerNode *) this;
    return that->findFunctionNode(name);
}

/*!
  Find the function node in this node that has the same name as \a clone.
 */
const FunctionNode *InnerNode::findFunctionNode(const FunctionNode *clone) const
{
    InnerNode *that = (InnerNode *) this;
    return that->findFunctionNode(clone);
}

/*!
 */
const EnumNode *InnerNode::findEnumNodeForValue(const QString &enumValue) const
{
    foreach (const Node *node, enumChildren) {
        const EnumNode *enume = static_cast<const EnumNode *>(node);
        if (enume->hasItem(enumValue))
            return enume;
    }
    return 0;
}

/*!
  Returnds the sequence number of the function node \a func
  in the list of overloaded functions for a class, such that
  all the functions have the same name as the \a func.
 */
int InnerNode::overloadNumber(const FunctionNode *func) const
{
    Node *node = (Node *) func;
    if (primaryFunctionMap[func->name()] == node) {
        return 1;
    }
    else {
        return secondaryFunctionMap[func->name()].indexOf(node) + 2;
    }
}

/*!
  Returns the number of member functions of a class such that
  the functions are all named \a funcName.
 */
int InnerNode::numOverloads(const QString& funcName) const
{
    if (primaryFunctionMap.contains(funcName)) {
        return secondaryFunctionMap[funcName].count() + 1;
    }
    else {
        return 0;
    }
}

/*!
  Returns a node list containing all the member functions of
  some class such that the functions overload the name \a funcName.
 */
NodeList InnerNode::overloads(const QString &funcName) const
{
    NodeList result;
    Node *primary = primaryFunctionMap.value(funcName);
    if (primary) {
        result << primary;
        result += secondaryFunctionMap[funcName];
    }
    return result;
}

/*!
  Construct an inner node (i.e., not a leaf node) of the
  given \a type and having the given \a parent and \a name.
 */
InnerNode::InnerNode(Type type, InnerNode *parent, const QString& name)
    : Node(type, parent, name)
{
    switch (type) {
    case Class:
    case Namespace:
        setPageType(ApiPage);
        break;
    default:
        break;
    }
}

/*!
  Appends an \a include file to the list of include files.
 */
void InnerNode::addInclude(const QString& include)
{
    inc.append(include);
}

/*!
  Sets the list of include files to \a includes.
 */
void InnerNode::setIncludes(const QStringList& includes)
{
    inc = includes;
}

/*!
  f1 is always the clone
 */
bool InnerNode::isSameSignature(const FunctionNode *f1, const FunctionNode *f2)
{
    if (f1->parameters().count() != f2->parameters().count())
        return false;
    if (f1->isConst() != f2->isConst())
        return false;

    QList<Parameter>::ConstIterator p1 = f1->parameters().begin();
    QList<Parameter>::ConstIterator p2 = f2->parameters().begin();
    while (p2 != f2->parameters().end()) {
        if ((*p1).hasType() && (*p2).hasType()) {
            if ((*p1).rightType() != (*p2).rightType())
                return false;

            QString t1 = p1->leftType();
            QString t2 = p2->leftType();

            if (t1.length() < t2.length())
                qSwap(t1, t2);

            /*
              ### hack for C++ to handle superfluous
              "Foo::" prefixes gracefully
            */
            if (t1 != t2 && t1 != (f2->parent()->name() + "::" + t2))
                return false;
        }
        ++p1;
        ++p2;
    }
    return true;
}

/*!
  Adds the \a child to this node's child list.
 */
void InnerNode::addChild(Node *child)
{
    children.append(child);
    if ((child->type() == Function) || (child->type() == QmlMethod)) {
        FunctionNode *func = (FunctionNode *) child;
        if (!primaryFunctionMap.contains(func->name())) {
            primaryFunctionMap.insert(func->name(), func);
        }
        else {
            NodeList &secs = secondaryFunctionMap[func->name()];
            secs.append(func);
        }
    }
    else {
        if (child->type() == Enum)
            enumChildren.append(child);
        childMap.insertMulti(child->name(), child);
    }
}

/*!
 */
void InnerNode::removeChild(Node *child)
{
    children.removeAll(child);
    enumChildren.removeAll(child);
    if (child->type() == Function) {
        QMap<QString, Node *>::Iterator prim =
                primaryFunctionMap.find(child->name());
        NodeList& secs = secondaryFunctionMap[child->name()];
        if (prim != primaryFunctionMap.end() && *prim == child) {
            if (secs.isEmpty()) {
                primaryFunctionMap.remove(child->name());
            }
            else {
                primaryFunctionMap.insert(child->name(), secs.takeFirst());
            }
        }
        else {
            secs.removeAll(child);
        }
    }
    QMap<QString, Node *>::Iterator ent = childMap.find(child->name());
    while (ent != childMap.end() && ent.key() == child->name()) {
        if (*ent == child) {
            childMap.erase(ent);
            break;
        }
        ++ent;
    }
}

/*!
  Find the module (QtCore, QtGui, etc.) to which the class belongs.
  We do this by obtaining the full path to the header file's location
  and examine everything between "src/" and the filename.  This is
  semi-dirty because we are assuming a particular directory structure.

  This function is only really useful if the class's module has not
  been defined in the header file with a QT_MODULE macro or with an
  \inmodule command in the documentation.
*/
QString Node::moduleName() const
{
    if (!mod.isEmpty())
        return mod;

    QString path = location().filePath();
    QString pattern = QString("src") + QDir::separator();
    int start = path.lastIndexOf(pattern);

    if (start == -1)
        return "";

    QString moduleDir = path.mid(start + pattern.size());
    int finish = moduleDir.indexOf(QDir::separator());

    if (finish == -1)
        return "";

    QString moduleName = moduleDir.left(finish);

    if (moduleName == "corelib")
        return "QtCore";
    else if (moduleName == "uitools")
        return "QtUiTools";
    else if (moduleName == "gui")
        return "QtGui";
    else if (moduleName == "network")
        return "QtNetwork";
    else if (moduleName == "opengl")
        return "QtOpenGL";
    else if (moduleName == "qt3support")
        return "Qt3Support";
    else if (moduleName == "svg")
        return "QtSvg";
    else if (moduleName == "sql")
        return "QtSql";
    else if (moduleName == "qtestlib")
        return "QtTest";
    else if (moduleDir.contains("webkit"))
        return "QtWebKit";
    else if (moduleName == "xml")
        return "QtXml";
    else
        return "";
}

/*!
 */
void InnerNode::removeRelated(Node *pseudoChild)
{
    related.removeAll(pseudoChild);
}

/*!
  \class LeafNode
 */

/*!
  Returns false because this is a LeafNode.
 */
bool LeafNode::isInnerNode() const
{
    return false;
}

/*!
  Constructs a leaf node named \a name of the specified
  \a type. The new leaf node becomes a child of \a parent.
 */
LeafNode::LeafNode(Type type, InnerNode *parent, const QString& name)
    : Node(type, parent, name)
{
    switch (type) {
    case Enum:
    case Function:
    case Typedef:
    case Variable:
    case QmlProperty:
    case QmlSignal:
    case QmlSignalHandler:
    case QmlMethod:
        setPageType(ApiPage);
        break;
    default:
        break;
    }
}

/*!
  \class NamespaceNode
 */

/*!
  Constructs a namespace node.
 */
NamespaceNode::NamespaceNode(InnerNode *parent, const QString& name)
    : InnerNode(Namespace, parent, name)
{
    setPageType(ApiPage);
}

/*!
  \class ClassNode
  \brief This class represents a C++ class.
 */

/*!
  Constructs a class node. A class node will generate an API page.
 */
ClassNode::ClassNode(InnerNode *parent, const QString& name)
    : InnerNode(Class, parent, name)
{
    hidden = false;
    abstract = false;
    qmlelement = 0;
    setPageType(ApiPage);
}

/*!
 */
void ClassNode::addBaseClass(Access access,
                             ClassNode *node,
                             const QString &dataTypeWithTemplateArgs)
{
    bases.append(RelatedClass(access, node, dataTypeWithTemplateArgs));
    node->derived.append(RelatedClass(access, this));
}

/*!
 */
void ClassNode::fixBaseClasses()
{
    int i;
    i = 0;
    QSet<ClassNode *> found;

    // Remove private and duplicate base classes.
    while (i < bases.size()) {
        ClassNode* bc = bases.at(i).node;
        if (bc->access() == Node::Private || found.contains(bc)) {
            RelatedClass rc = bases.at(i);
            bases.removeAt(i);
            ignoredBases.append(rc);
            const QList<RelatedClass> &bb = bc->baseClasses();
            for (int j = bb.size() - 1; j >= 0; --j)
                bases.insert(i, bb.at(j));
        }
        else {
            ++i;
        }
        found.insert(bc);
    }

    i = 0;
    while (i < derived.size()) {
        ClassNode* dc = derived.at(i).node;
        if (dc->access() == Node::Private) {
            derived.removeAt(i);
            const QList<RelatedClass> &dd = dc->derivedClasses();
            for (int j = dd.size() - 1; j >= 0; --j)
                derived.insert(i, dd.at(j));
        }
        else {
            ++i;
        }
    }
}

/*!
  Search the child list to find the property node with the
  specified \a name.
 */
const PropertyNode *ClassNode::findPropertyNode(const QString &name) const
{
    const Node *n = findNode(name, Node::Property);

    if (n)
        return static_cast<const PropertyNode*>(n);

    const PropertyNode *pn = 0;

    const QList<RelatedClass> &bases = baseClasses();
    if (!bases.isEmpty()) {
        for (int i = 0; i < bases.size(); ++i) {
            const ClassNode *cn = bases[i].node;
            pn = cn->findPropertyNode(name);
            if (pn)
                break;
        }
    }
    const QList<RelatedClass>& ignoredBases = ignoredBaseClasses();
    if (!ignoredBases.isEmpty()) {
        for (int i = 0; i < ignoredBases.size(); ++i) {
            const ClassNode *cn = ignoredBases[i].node;
            pn = cn->findPropertyNode(name);
            if (pn)
                break;
        }
    }

    return pn;
}

/*!
  This function does a recursive search of this class node's
  base classes looking for one that has a QML element. If it
  finds one, it returns the pointer to that QML element. If
  it doesn't find one, it returns null.
 */
const QmlClassNode* ClassNode::findQmlBaseNode() const
{
    const QmlClassNode* result = 0;
    const QList<RelatedClass>& bases = baseClasses();

    if (!bases.isEmpty()) {
        for (int i = 0; i < bases.size(); ++i) {
            const ClassNode* cn = bases[i].node;
            if (cn && cn->qmlElement()) {
                return cn->qmlElement();
            }
        }
        for (int i = 0; i < bases.size(); ++i) {
            const ClassNode* cn = bases[i].node;
            if (cn) {
                result = cn->findQmlBaseNode();
                if (result != 0) {
                    return result;
                }
            }
        }
    }
    return result;
}

/*!
  \class FakeNode
 */

/*!
  The type of a FakeNode is Fake, and it has a \a subtype,
  which specifies the type of FakeNode. The page type for
  the page index is set here.
 */
FakeNode::FakeNode(InnerNode* parent, const QString& name, SubType subtype, Node::PageType ptype)
    : InnerNode(Fake, parent, name), sub(subtype)
{
    switch (subtype) {
    case Page:
        setPageType(ptype);
        break;
    case Module:
    case QmlModule:
    case Group:
        setPageType(OverviewPage);
        break;
    case QmlClass:
    case QmlBasicType:
        setPageType(ApiPage);
        break;
    case Example:
        setPageType(ExamplePage);
        break;
    case Collision:
        setPageType(ptype);
        break;
    default:
        break;
    }
}

/*!
  Returns the fake node's title. This is used for the page title.
*/
QString FakeNode::title() const
{
    return tle;
}

/*!
  Returns the fake node's full title, which is usually
  just title(), but for some SubType values is different
  from title()
 */
QString FakeNode::fullTitle() const
{
    if (sub == File) {
        if (title().isEmpty())
            return name().mid(name().lastIndexOf('/') + 1) + " Example File";
        else
            return title();
    }
    else if (sub == Image) {
        if (title().isEmpty())
            return name().mid(name().lastIndexOf('/') + 1) + " Image File";
        else
            return title();
    }
    else if ((sub == HeaderFile) || (sub == Collision)) {
        if (title().isEmpty())
            return name();
        else
            return name() + " - " + title();
    }
    else {
        return title();
    }
}

/*!
  Returns the subtitle.
 */
QString FakeNode::subTitle() const
{
    if (!stle.isEmpty())
        return stle;

    if ((sub == File) || (sub == Image)) {
        if (title().isEmpty() && name().contains("/"))
            return name();
    }
    return QString();
}

/*!
  The constructor calls the FakeNode constructor with
  \a parent, \a name, and Node::Example.
 */
ExampleNode::ExampleNode(InnerNode* parent, const QString& name)
    : FakeNode(parent, name, Node::Example, Node::ExamplePage)
{
    // nothing
}

/*!
  \class EnumNode
 */

/*!
  The constructor for the node representing an enum type
  has a \a parent class and an enum type \a name.
 */
EnumNode::EnumNode(InnerNode *parent, const QString& name)
    : LeafNode(Enum, parent, name), ft(0)
{
}

/*!
  Add \a item to the enum type's item list.
 */
void EnumNode::addItem(const EnumItem& item)
{
    itms.append(item);
    names.insert(item.name());
}

/*!
  Returns the access level of the enumeration item named \a name.
  Apparently it is private if it has been omitted by qdoc's
  omitvalue command. Otherwise it is public.
 */
Node::Access EnumNode::itemAccess(const QString &name) const
{
    if (doc().omitEnumItemNames().contains(name))
        return Private;
    return Public;
}

/*!
  Returns the enum value associated with the enum \a name.
 */
QString EnumNode::itemValue(const QString &name) const
{
    foreach (const EnumItem &item, itms) {
        if (item.name() == name)
            return item.value();
    }
    return QString();
}

/*!
  \class TypedefNode
 */

/*!
 */
TypedefNode::TypedefNode(InnerNode *parent, const QString& name)
    : LeafNode(Typedef, parent, name), ae(0)
{
}

/*!
 */
void TypedefNode::setAssociatedEnum(const EnumNode *enume)
{
    ae = enume;
}

/*!
  \class Parameter
  \brief The class Parameter contains one parameter.

  A parameter can be a function parameter or a macro
  parameter.
 */

/*!
  Constructs this parameter from the left and right types
  \a leftType and rightType, the parameter \a name, and the
  \a defaultValue. In practice, \a rightType is not used,
  and I don't know what is was meant for.
 */
Parameter::Parameter(const QString& leftType,
                     const QString& rightType,
                     const QString& name,
                     const QString& defaultValue)
    : lef(leftType), rig(rightType), nam(name), def(defaultValue)
{
}

/*!
  The standard copy constructor copies the strings from \a p.
 */
Parameter::Parameter(const Parameter& p)
    : lef(p.lef), rig(p.rig), nam(p.nam), def(p.def)
{
}

/*!
  Assigning Parameter \a p to this Parameter copies the
  strings across.
 */
Parameter& Parameter::operator=(const Parameter& p)
{
    lef = p.lef;
    rig = p.rig;
    nam = p.nam;
    def = p.def;
    return *this;
}

/*!
  Reconstructs the text describing the parameter and
  returns it. If \a value is true, the default value
  will be included, if there is one.
 */
QString Parameter::reconstruct(bool value) const
{
    QString p = lef + rig;
    if (!p.endsWith(QChar('*')) && !p.endsWith(QChar('&')) && !p.endsWith(QChar(' ')))
        p += " ";
    p += nam;
    if (value && !def.isEmpty())
        p += " = " + def;
    return p;
}


/*!
  \class FunctionNode
 */

/*!
  Construct a function node for a C++ function. It's parent
  is \a parent, and it's name is \a name.
 */
FunctionNode::FunctionNode(InnerNode *parent, const QString& name)
    : LeafNode(Function, parent, name),
      met(Plain),
      vir(NonVirtual),
      con(false),
      sta(false),
      ove(false),
      reimp(false),
      att(false),
      rf(0),
      ap(0)
{
    // nothing.
}

/*!
  Construct a function node for a QML method or signal, specified
  by \a type. It's parent is \a parent, and it's name is \a name.
  If \a attached is true, it is an attached method or signal.
 */
FunctionNode::FunctionNode(Type type, InnerNode *parent, const QString& name, bool attached)
    : LeafNode(type, parent, name),
      met(Plain),
      vir(NonVirtual),
      con(false),
      sta(false),
      ove(false),
      reimp(false),
      att(attached),
      rf(0),
      ap(0)
{
    // nothing.
}

/*!
  Sets the \a virtualness of this function. If the \a virtualness
  is PureVirtual, and if the parent() is a ClassNode, set the parent's
  \e abstract flag to true.
 */
void FunctionNode::setVirtualness(Virtualness virtualness)
{
    vir = virtualness;
    if ((virtualness == PureVirtual) && parent() &&
        (parent()->type() == Node::Class))
        parent()->setAbstract(true);
}

/*!
 */
void FunctionNode::setOverload(bool overlode)
{
    parent()->setOverload(this, overlode);
    ove = overlode;
}

/*!
  Sets the function node's reimplementation flag to \a r.
  When \a r is true, it is supposed to mean that this function
  is a reimplementation of a virtual function in a base class,
  but it really just means the \e reimp command was seen in the
  qdoc comment.
 */
void FunctionNode::setReimp(bool r)
{
    reimp = r;
}

/*!
 */
void FunctionNode::addParameter(const Parameter& parameter)
{
    params.append(parameter);
}

/*!
 */
void FunctionNode::borrowParameterNames(const FunctionNode *source)
{
    QList<Parameter>::Iterator t = params.begin();
    QList<Parameter>::ConstIterator s = source->params.begin();
    while (s != source->params.end() && t != params.end()) {
        if (!(*s).name().isEmpty())
            (*t).setName((*s).name());
        ++s;
        ++t;
    }
}

/*!
  If this function is a reimplementation, \a from points
  to the FunctionNode of the function being reimplemented.
 */
void FunctionNode::setReimplementedFrom(FunctionNode *from)
{
    rf = from;
    from->rb.append(this);
}

/*!
  Sets the "associated" property to \a property. The function
  might be the setter or getter for a property, for example.
 */
void FunctionNode::setAssociatedProperty(PropertyNode *property)
{
    ap = property;
}

/*!
  Returns the overload number for this function obtained
  from the parent.
 */
int FunctionNode::overloadNumber() const
{
    return parent()->overloadNumber(this);
}

/*!
  Returns the number of times this function name has been
  overloaded, obtained from the parent.
 */
int FunctionNode::numOverloads() const
{
    return parent()->numOverloads(name());
}

/*!
  Returns the list of parameter names.
 */
QStringList FunctionNode::parameterNames() const
{
    QStringList names;
    QList<Parameter>::ConstIterator p = parameters().begin();
    while (p != parameters().end()) {
        names << (*p).name();
        ++p;
    }
    return names;
}

/*!
  Returns a raw list of parameters. If \a names is true, the
  names are included. If \a values is true, the default values
  are included, if any are present.
 */
QString FunctionNode::rawParameters(bool names, bool values) const
{
    QString raw;
    foreach (const Parameter &parameter, parameters()) {
        raw += parameter.leftType() + parameter.rightType();
        if (names)
            raw += parameter.name();
        if (values)
            raw += parameter.defaultValue();
    }
    return raw;
}

/*!
  Returns the list of reconstructed parameters. If \a values
  is true, the default values are included, if any are present.
 */
QStringList FunctionNode::reconstructParams(bool values) const
{
    QStringList params;
    QList<Parameter>::ConstIterator p = parameters().begin();
    while (p != parameters().end()) {
        params << (*p).reconstruct(values);
        ++p;
    }
    return params;
}

/*!
  Reconstructs and returns the function's signature. If \a values
  is true, the default values of the parameters are included, if
  present.
 */
QString FunctionNode::signature(bool values) const
{
    QString s;
    if (!returnType().isEmpty())
        s = returnType() + " ";
    s += name() + "(";
    QStringList params = reconstructParams(values);
    int p = params.size();
    if (p > 0) {
        for (int i=0; i<p; i++) {
            s += params[i];
            if (i < (p-1))
                s += ", ";
        }
    }
    s += ")";
    return s;
}

/*!
  Returns true if the node's status is Internal, or if its
  parent is a class with internal status.
 */
bool FunctionNode::isInternal() const
{
    if (status() == Internal)
        return true;
    if (parent() && parent()->status() == Internal)
        return true;
    if (relates() && relates()->status() == Internal)
        return true;
    return false;
}

/*!
  Print some debugging stuff.
 */
void FunctionNode::debug() const
{
    qDebug("QML METHOD %s rt %s pp %s",
            qPrintable(name()), qPrintable(rt), qPrintable(pp.join(" ")));
}

/*!
  \class PropertyNode

  This class describes one instance of using the Q_PROPERTY macro.
 */

/*!
  The constructor sets the \a parent and the \a name, but
  everything else is set to default values.
 */
PropertyNode::PropertyNode(InnerNode *parent, const QString& name)
    : LeafNode(Property, parent, name),
      sto(Trool_Default),
      des(Trool_Default),
      scr(Trool_Default),
      wri(Trool_Default),
      usr(Trool_Default),
      cst(false),
      fnl(false),
      rev(-1),
      overrides(0)
{
    // nothing.
}

/*!
  Sets this property's \e {overridden from} property to
  \a baseProperty, which indicates that this property
  overrides \a baseProperty. To begin with, all the values
  in this property are set to the corresponding values in
  \a baseProperty.

  We probably should ensure that the constant and final
  attributes are not being overridden improperly.
 */
void PropertyNode::setOverriddenFrom(const PropertyNode* baseProperty)
{
    for (int i = 0; i < NumFunctionRoles; ++i) {
        if (funcs[i].isEmpty())
            funcs[i] = baseProperty->funcs[i];
    }
    if (sto == Trool_Default)
        sto = baseProperty->sto;
    if (des == Trool_Default)
        des = baseProperty->des;
    if (scr == Trool_Default)
        scr = baseProperty->scr;
    if (wri == Trool_Default)
        wri = baseProperty->wri;
    if (usr == Trool_Default)
        usr = baseProperty->usr;
    overrides = baseProperty;
}

/*!
 */
QString PropertyNode::qualifiedDataType() const
{
    if (setters().isEmpty() && resetters().isEmpty()) {
        if (dt.contains("*") || dt.contains("&")) {
            // 'QWidget *' becomes 'QWidget *' const
            return dt + " const";
        }
        else {
            /*
              'int' becomes 'const int' ('int const' is
              correct C++, but looks wrong)
            */
            return "const " + dt;
        }
    }
    else {
        return dt;
    }
}

/*! Converts the \a boolean value to an enum representation
  of the boolean type, which includes an enum  value for the
  \e {default value} of the item, i.e. true, false, or default.
 */
PropertyNode::Trool PropertyNode::toTrool(bool boolean)
{
    return boolean ? Trool_True : Trool_False;
}

/*!
  Converts the enum \a troolean back to a boolean value.
  If \a troolean is neither the true enum value nor the
  false enum value, the boolean value returned is
  \a defaultValue.

  Note that runtimeDesignabilityFunction() should be called
  first. If that function returns the name of a function, it
  means the function must be called at runtime to determine
  whether the property is Designable.
 */
bool PropertyNode::fromTrool(Trool troolean, bool defaultValue)
{
    switch (troolean) {
    case Trool_True:
        return true;
    case Trool_False:
        return false;
    default:
        return defaultValue;
    }
}

/*!
  \class TargetNode
 */

/*!
 */
TargetNode::TargetNode(InnerNode *parent, const QString& name)
    : LeafNode(Target, parent, name)
{
}

/*!
  Returns false because this is a TargetNode.
 */
bool TargetNode::isInnerNode() const
{
    return false;
}

bool QmlClassNode::qmlOnly = false;
QMultiMap<QString,Node*> QmlClassNode::inheritedBy;
QMap<QString, QmlClassNode*> QmlClassNode::moduleMap;

/*!
  Constructs a Qml class node (i.e. a Fake node with the
  subtype QmlClass. The new node has the given \a parent
  and \a name and is associated with the C++ class node
  specified by \a cn which may be null if the the Qml
  class node is not associated with a C++ class node.
 */
QmlClassNode::QmlClassNode(InnerNode *parent,
                           const QString& name,
                           const ClassNode* cn)
    : FakeNode(parent, name, QmlClass, Node::ApiPage),
      cnode(cn),
      base_(0)
{
    int i = 0;
    if (name.startsWith("QML:")) {
        qDebug() << "BOGUS:" << name;
        i = 4;
    }
    setTitle(name.mid(i) + QLatin1String(" Reference"));
}

/*!
  I made this so I could print a debug message here.
 */
QmlClassNode::~QmlClassNode()
{
#ifdef DEBUG_MULTIPLE_QDOCCONF_FILES
    qDebug() << "Deleting QmlClassNode:" << name();
#endif
}

/*!
  Clear the static maps so that subsequent runs don't try to use
  contents from a previous run.
 */
void QmlClassNode::terminate()
{
    inheritedBy.clear();
    moduleMap.clear();
}

/*!
  The base file name for this kind of node has "qml_"
  prepended to it.

  But not yet. Still testing.
 */
QString QmlClassNode::fileBase() const
{
    return Node::fileBase();
}

/*!
  Record the fact that QML class \a base is inherited by
  QML class \a sub.
 */
void QmlClassNode::addInheritedBy(const QString& base, Node* sub)
{
    if (inheritedBy.find(base,sub) == inheritedBy.end()) {
        inheritedBy.insert(base,sub);
     }
#ifdef DEBUG_MULTIPLE_QDOCCONF_FILES
    qDebug() << "QmlClassNode::addInheritedBy(): insert" << base << sub->name() << inheritedBy.size();
#endif
}

/*!
  Loads the list \a subs with the nodes of all the subclasses of \a base.
 */
void QmlClassNode::subclasses(const QString& base, NodeList& subs)
{
    subs.clear();
    if (inheritedBy.count(base) > 0) {
        subs = inheritedBy.values(base);
#ifdef DEBUG_MULTIPLE_QDOCCONF_FILES
        qDebug() << "QmlClassNode::subclasses():" <<  inheritedBy.count(base) << base
                 << "subs:" << subs.size() << "total size:" << inheritedBy.size();
#endif
    }
}

/*! \fn QString QmlClassNode::qmlModuleQualifier() const
  This function is called to get a string that is used either
  as a prefix for the file name to use for QML element or
  component reference page, or as a qualifier to prefix a
  reference to a QML element or comnponent. The string that
  is returned is the concatenation of the QML module name
  and its version number. e.g., if an element or component
  is defined to be in the QML module QtQuick 1, its module
  qualifier is "QtQuick1". See setQmlModuleName().
 */

/*! \fn QString QmlClassNode::qmlModuleIdentifier() const
  This function is the same as qmlModuleQualifier() except
  that this one returns a string with the module name and
  the version number separated by a single blank. e.g., if
  an element or component is defined to be in QML module
  QtQuick 1, its module identifier is "QtQuick 1". See
  qmlModuleQualifier().
 */

/*!
  This function splits \a arg on the blank character to get a
  QML module name and version number. It stores these separately.
  The version number is not required.
 */
void QmlClassNode::setQmlModuleName(const QString& arg)
{
    QStringList blankSplit = arg.split(" ");
    qmlModuleName_ = blankSplit[0];
    if (blankSplit.size() > 1)
        qmlModuleVersion_ = blankSplit[1];
}

/*!
  The name of this QML class node might be the same as the
  name of some other QML class node. If so, then this node's
  parent will be a NameCollisionNode.This function sets the
  NameCollisionNode's current child to this node. This is
  important when outputing the documentation for this node,
  when, for example, the documentation contains a link to
  the page being output. We don't want to generate a link
  to the disambiguation page if we can avoid it, and to be
  able to avoid it, the NameCollisionNode must maintain the
  current child pointer. That's the purpose of this function.
 */
void QmlClassNode::setCurrentChild()
{
    if (parent()) {
        InnerNode* n = parent();
        if (n->subType() == Node::Collision)
            n->setCurrentChild(this);
    }
}

/*!
 */
void QmlClassNode::clearCurrentChild()
{
    if (parent()) {
        InnerNode* n = parent();
        if (n->subType() == Node::Collision)
            n->clearCurrentChild();
    }
}

/*!
  Most QML elements don't have an \\inherits command in their
  \\qmlclass command. This leaves qdoc bereft, when it tries
  to output the line in the documentation that specifies the
  QML element that a QML element inherits.

 */
void QmlClassNode::resolveInheritance(const Tree* tree)
{
    bool debug = false;
    if (!links().empty() && links().contains(Node::InheritsLink)) {
        QPair<QString,QString> linkPair;
        linkPair = links()[Node::InheritsLink];
        QStringList strList = linkPair.first.split("::");
        const Node* n = tree->findNode(strList,Node::Fake);
        if (n && (n->subType() == Node::QmlClass || n->subType() == Node::Collision)) {
            base_ = static_cast<const FakeNode*>(n);
            if (base_ && base_->subType() == Node::QmlClass) {
                if (debug)
                    qDebug() << qmlModuleIdentifier() << name() << "INHERITS"
                             << base_->qmlModuleIdentifier() << base_->name();
                return;
            }
        }
        if (base_ && base_->subType() == Node::Collision) {
            const NameCollisionNode* ncn = static_cast<const NameCollisionNode*>(base_);
            const NodeList& children = ncn->childNodes();
            for (int i=0; i<importList_.size(); ++i) {
                QString qmq = importList_.at(i).first + importList_.at(i).second;
                for (int j=0; j<children.size(); ++j) {
                    if (qmq == children.at(j)->qmlModuleQualifier()) {
                        base_ = static_cast<const FakeNode*>(children.at(j));
                        if (debug)
                            qDebug() << qmlModuleIdentifier() << name() << "INHERITS"
                                     << base_->qmlModuleIdentifier() << base_->name();
                        return;
                    }
                }
            }
            QString qmid = qmlModuleIdentifier();
            for (int k=0; k<children.size(); ++k) {
                if (qmid == children.at(k)->qmlModuleIdentifier()) {
                    base_ = static_cast<const QmlClassNode*>(children.at(k));
                    if (debug)
                        qDebug() << qmlModuleIdentifier() << name() << "INHERITS"
                                 << base_->qmlModuleIdentifier() << base_->name();
                    return;
                }
            }
        }
        if (base_) {
            if (base_->subType() == Node::Collision) {
                const NameCollisionNode* ncn = static_cast<const NameCollisionNode*>(base_);
                const NodeList& children = ncn->childNodes();
                if (debug) {
                    qDebug() << "QML CLASS:" << qmlModuleQualifier() << name();
                    qDebug() << "  COLLISION:" << base_->name();
                    for (int j=0; j<children.size(); ++j) {
                        qDebug() << "    " << children.at(j)->qmlModuleQualifier()
                                 << children.at(j)->name();
                    }
                }
            }
            return;
        }
    }
    if (cnode) {
        const QmlClassNode* qcn = cnode->findQmlBaseNode();
        if (qcn != 0) {
            base_ = qcn;
            if (debug)
                qDebug() << qmlModuleIdentifier() << name() << "INHERITS"
                         << base_->qmlModuleIdentifier() << base_->name();
        }
        else {
            if (debug)
                qDebug() << qmlModuleIdentifier() << name() << "doesn't inherit anything";
        }
    }
    return;
}

/*!
  Constructs a Qml basic type node (i.e. a Fake node with
  the subtype QmlBasicType. The new node has the given
  \a parent and \a name.
 */
QmlBasicTypeNode::QmlBasicTypeNode(InnerNode *parent,
                                   const QString& name)
    : FakeNode(parent, name, QmlBasicType, Node::ApiPage)
{
    setTitle(name);
}

/*!
  Constructor for the Qml property group node. \a parent is
  always a QmlClassNode.
 */
QmlPropGroupNode::QmlPropGroupNode(QmlClassNode* parent,
                                   const QString& name,
                                   bool attached)
    : FakeNode(parent, name, QmlPropertyGroup, Node::ApiPage),
      isdefault(false),
      att(attached)
{
    // nothing.
}

/*!
  Constructor for the QML property node.
 */
QmlPropertyNode::QmlPropertyNode(QmlPropGroupNode *parent,
                                 const QString& name,
                                 const QString& type,
                                 bool attached)
    : LeafNode(QmlProperty, parent, name),
      dt(type),
      sto(Trool_Default),
      des(Trool_Default),
      isdefault(false),
      att(attached)
{
    setPageType(ApiPage);
}

/*!
  Constructor for the QML property node.
 */
QmlPropertyNode::QmlPropertyNode(QmlClassNode *parent,
                                 const QString& name,
                                 const QString& type,
                                 bool attached)
    : LeafNode(QmlProperty, parent, name),
      dt(type),
      sto(Trool_Default),
      des(Trool_Default),
      isdefault(false),
      att(attached)
{
    setPageType(ApiPage);
}

/*!
  I don't know what this is.
 */
QmlPropertyNode::Trool QmlPropertyNode::toTrool(bool boolean)
{
    return boolean ? Trool_True : Trool_False;
}

/*!
  I don't know what this is either.
 */
bool QmlPropertyNode::fromTrool(Trool troolean, bool defaultValue)
{
    switch (troolean) {
    case Trool_True:
        return true;
    case Trool_False:
        return false;
    default:
        return defaultValue;
    }
}

/*!
  Returns true if a QML property or attached property is
  read-only. The algorithm for figuring this out is long
  amd tedious and almost certainly will break. It currently
  doesn't work for qmlproperty bool PropertyChanges::explicit,
  because the tokenizer gets confused on "explicit".
 */
bool QmlPropertyNode::isWritable(const Tree* tree) const
{
    if (wri != Trool_Default)
        return fromTrool(wri, false);

    const PropertyNode *pn = correspondingProperty(tree);
    if (pn)
        return pn->isWritable();
    else {
        location().warning(tr("Can't determine read-only status of QML property %1; writable assumed.").arg(name()));
        return true;
    }
}

const PropertyNode *QmlPropertyNode::correspondingProperty(const Tree *tree) const
{
    const PropertyNode *pn;

    Node* n = parent();
    while (n && n->subType() != Node::QmlClass)
        n = n->parent();
    if (n) {
        const QmlClassNode* qcn = static_cast<const QmlClassNode*>(n);
        const ClassNode* cn = qcn->classNode();
        if (cn) {
            QStringList dotSplit = name().split(QChar('.'));
            pn = cn->findPropertyNode(dotSplit[0]);
            if (pn) {
                if (dotSplit.size() > 1) {
                    // Find the C++ property corresponding to the QML property in
                    // the property group, <group>.<property>.

                    QStringList path(extractClassName(pn->qualifiedDataType()));
                    const Node* nn = tree->findNode(path,Class);
                    if (nn) {
                        const ClassNode* cn = static_cast<const ClassNode*>(nn);
                        const PropertyNode *pn2 = cn->findPropertyNode(dotSplit[1]);
                        if (pn2)
                            return pn2; // Return the property for the QML property.
                        else
                            return pn;  // Return the property for the QML group.
                    }
                }
                else
                    return pn;
            }
            else {
                pn = cn->findPropertyNode(dotSplit[0]);
                if (pn)
                    return pn;
            }
        }
    }

    return 0;
}

/*! \class NameCollisionNode

  An instance of this node is inserted in the tree
  whenever qdoc discovers that two nodes have the
  same name.
 */

/*!
  Constructs a name collision node containing \a child
  as its first child. The parent of \a child becomes
  this node's parent.
 */
NameCollisionNode::NameCollisionNode(InnerNode* child)
    : FakeNode(child->parent(), child->name(), Collision, Node::NoPageType)
{
    addCollision(child);
    current = 0;
}

/*!
  Add a collision to this collision node. \a child has
  the same name as the other children in this collision
  node. \a child becomes the current child.
 */
void NameCollisionNode::addCollision(InnerNode* child)
{
    if (child) {
        if (child->parent())
            child->parent()->removeChild(child);
        child->setParent((InnerNode*)this);
        children.append(child);
    }
}

/*!
  The destructor does nothing.
 */
NameCollisionNode::~NameCollisionNode()
{
    // nothing.
}

/*! \fn const InnerNode* NameCollisionNode::currentChild() const
  Returns a pointer to the current child, which may be 0.
 */

/*! \fn void NameCollisionNode::setCurrentChild(InnerNode* child)
  Sets the current child to \a child. The current child is
  valid only within the file where it is defined.
 */

/*! \fn void NameCollisionNode::clearCurrentChild()
  Sets the current child to 0. This should be called at the
  end of each file, because the current child is only valid
  within the file where the child is defined.
 */

/*!
  Returns true if this collision node's current node is a QML node.
 */
bool NameCollisionNode::isQmlNode() const
{
    if (current)
        return current->isQmlNode();
    return false;
}

/*!
  Find any of this collision node's children that has type \a t
  and subtype \a st. Normally, it will be the current child, but
  the return value might be null. It never should be null, but
  at this point, I'm not sure what will happen.
*/
const InnerNode* NameCollisionNode::findAny(Node::Type t, Node::SubType st) const
{
    if (current) {
        if (current->type() == t && current->subType() == st)
            return current;
    }
    const NodeList& cn = childNodes();
    NodeList::ConstIterator i = cn.begin();
    while (i != cn.end()) {
        if ((*i)->type() == t && (*i)->subType() == st)
            return static_cast<const InnerNode*>(*i);
        ++i;
    }
    return 0;
}

QT_END_NAMESPACE
