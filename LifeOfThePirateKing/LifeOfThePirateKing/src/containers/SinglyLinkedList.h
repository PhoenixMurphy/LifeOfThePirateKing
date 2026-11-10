#pragma once

#include <cstddef>

//This is useed for the level sequencing
template <typename T>
class SinglyLinkedList
{
	struct Node
	{
		T data;
		Node* next = nullptr;
	};

	SinglyLinkedList() = default;
	~SinglyLinkedList() { Clear() };

	// The class owns the raw pointers, no copying
	SinglyLinkedList(const SinglyLinkedList&) = delete;
	SinglyLinkedList& operator=(const SinglyLinkedList&) = delete;

	// Add to the end
	Node* PushBack(const T& value)
	{
		Node* node = new Node{ value, nullptr };

		if (m_tail == nullptr)
		{
			m_head = node;
		}
		else
		{
			m_tail->next = node;
		}

		m_tail = node;
		++m_size;
		return node;
	}

	// Add to the start
	Node* PushFront(const T& value)
	{
		Node* node = new Node{ value, m_head };

		m_head = node;
		if (m_tail == nullptr)
		{
			m_tail = node;
		}

		++m_size;
		return node;
	}

	// Insert after one node
	Node* InsertAfter(Node* node, const T& value)
	{
		if (node == nullptr)
		{
			return nullptr;
		}

		Node* inserted = new Node{ value, node->next };

		node->next = inserted;

		if (node == m_tail)
		{
			m_tail = inserted;
		}

		++m_size;
		return inserted;
	}

	void Clear()
	{
		Node* node = m_head;
		
		while (node != nullptr)
		{
			Node* next = node->next;
			delete node;
			node = next;
		}

		m_head = nullptr;
		m_tail = nullptr;
		m_size = 0;
	}

	Node* Head()
	{
		return m_head;
	}

	const Node* Head() const
	{
		return m_head;
	}

	Node* Tail()
	{
		return m_tail;
	}

	const Node* Tail() const
	{
		return m_tail;
	}

	bool Empty()
	{
		return m_head == nullptr;
	}

	std::size_t Size() const
	{
		return m_size;
	}
private:
	Node* m_head = nullptr;
	Node* m_tail = nullptr;
	std::size_t m_size = 0;
};