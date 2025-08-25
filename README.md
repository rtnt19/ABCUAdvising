Rita (Nathaniel) Tetterton
SNHU CS-300
Professor Nathan Lebel
Reflection
What problem was I solving?

Across these projects, I built components of a course management system that load, validate, store, and present Computer Science course data. The core problems were: choosing appropriate data structures to represent courses and prerequisites, analyzing their time/space trade‑offs for operations like insert, lookup, and traversal, and implementing a program to sort and print courses alphanumerically with reliable parsing and validation.
How did I approach the problem and why are data structures important?

I started by mapping operations to data-structure strengths. For sequential loading and global ordering, a vector provides cache-friendly iteration and straightforward sorting. For flexible insertion/removal without shifting elements, linked lists help at the cost of O(n) search. For fast keyed access by course number, a hash table gives average O(1) inserts/lookups if the load factor is controlled and hashing is consistent. Thinking in terms of operations and constraints—rather than code first—kept me focused on performance characteristics and correctness invariants.
How did I overcome roadblocks?

Linked list pointer management was the steepest learning curve. I systematically tested head/tail updates and boundary cases: empty list, single element, inserting/removing at the beginning and end, and removing non-existent elements. During file parsing, I added input validation and clear error messages to handle malformed lines and missing fields, then iterated on small samples before scaling up. For the hash table, tuning the load factor and clarifying hashing/equality rules eliminated intermittent lookup bugs.
How did this expand my approach to designing software?

I now separate concerns into clear stages: parsing and validation, storage and indexing, and presentation/printing. I prototype data flows with minimal datasets, measure the costs of core operations, and pick structures that match usage patterns. This mindset made it easier to swap implementations—for example, moving from a vector-backed lookup to a hash table when lookup frequency grew—without rewriting the entire pipeline.
How did this evolve the way I write maintainable, readable, and adaptable code?

I leaned into cohesive abstractions (Course, Parser, Catalog/HashTable), documented invariants (e.g., head/tail must remain consistent after insert/delete), and wrote guardrails around input handling. Clear naming, small functions with single responsibilities, and comments capturing pre/postconditions made future changes safer. I also introduced straightforward tests for edge cases and standardized output formatting, so modifying sort keys or adding features (like prerequisite checks) is localized and low-risk.




