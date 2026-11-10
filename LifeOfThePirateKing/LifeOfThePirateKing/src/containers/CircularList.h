#pragma once

#include <cstddef>

// This is used for rotating between the four arena gates
// when spawning enemies and looping sprite animation frames.

template <typename T>
class CircularList
{
public:
	struct Node
	{
		T data;
		Node* next = nullptr;
	};

	CircularList() = default;
	~CircularList() { Clear(); }

	// The list owns its own nodes, this prevents copying
	CircularList(const CircularList&) = delete;
	CircularList& operator=(const CircularList&) = delete;

	// Add a value before the head
	void Push(const T& value)
	{
		Node* node = new Node{ value, nullptr };

		if (m_tail == nullptr)
		{
			node->next = node;
			m_current = node;
		}
		else
		{
			node->next = m_tail->next;
			m_tail->next = node;
		}

		m_tail = node;
		++m_size;
	}

	// Moves the current node one step, after the last element it
	// goes to the first one again
	void Advance()
	{
		if (m_tail == nullptr)
		{
			m_current = m_current->next;
		}
	}

	// The element in the current node, nullptr if it is empty
	T* Current()
	{
		if (m_current != nullptr)
		{
			return &m_current->data;
		}
		else
		{
			return nullptr;
		}
	}

	const T* Current() const
	{
		if (m_current != nullptr)
		{
			return &m_current->data;
		}
		else
		{
			return nullptr;
		}
	}

	// Restarts an animation
	void ResetCursor()
	{
		if (m_tail != nullptr)
		{
			m_current = m_tail->next;
		}
		else
		{
			m_current = nullptr;
		}
	}

	void Clear()
	{
		if (m_tail == nullptr)
		{
			return;
		}

		// Break the ring otherwise the loop would never
		// reach nullptr
		Node* node = m_tail->next;
		m_tail->next = nullptr;

		while (node != nullptr)
		{
			Node* next = node->next;

			delete node;
			node = next;
		}

		m_tail = nullptr;
		m_current = nullptr;
		m_size = 0;
	}

	bool Empty() const
	{
		return m_tail == nullptr;
	}

	std::size_t Size()
	{
		return m_size
	}

private:
	Node* m_tail = nullptr;
	Node* m_current = nullptr;
	std::size_t m_size = 0;
};